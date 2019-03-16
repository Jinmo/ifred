#include "plugin.h"
#include <pybind11/pybind11.h>

#include <QtGui>
#include <QtWidgets>

#include <widgets/qpalettecontainer.h>
#include <widgets/palette_manager.h>

QHash<QString, QDate> g_last_used;

QVector<Action> cached_actions;

QVector<QRegularExpression> getBlacklist() {
	auto blacklist = json("config.json")["blacklist"].toArray();
	QVector<QRegularExpression> blacklist_converted;

	for (auto& i : blacklist) {
		if (i.toString().size())
			blacklist_converted.push_back(QRegularExpression(i.toString()));
	}
	return blacklist_converted;
}

const QVector<Action> getActions() {
	qstrvec_t id_list;
	get_registered_actions(&id_list);

	size_t names_count = get_nlist_size();

	if (cached_actions.size() == id_list.size() + names_count)
		return std::move(QVector<Action>(cached_actions));

	QVector<Action> result;
	auto blacklist = getBlacklist();

	result.reserve(id_list.size());

	// Variables used in the loop below
	qstring tooltip, shortcut;
	action_state_t state;

	for (auto& item : id_list) {
		bool skip = false;
		for (auto& pattern : blacklist)
			if (pattern.match(item.c_str()).hasMatch()) {
				skip = true;
				break;
			}

		if (skip)
			continue;

		if (!get_action_state(item.c_str(), &state))
			continue;

		if (state > AST_ENABLE)
			continue;

		get_action_tooltip(&tooltip, item.c_str());
		get_action_shortcut(&shortcut, item.c_str());
		result.push_back(Action(
			QString(item.c_str()), QString(tooltip.c_str()), QString(shortcut.c_str())));
	}

	for (size_t i = 0; i < names_count; i++) {
		result.push_back(Action(
			"@ " + QString::number(get_nlist_ea(i), 16).toUpper(),
			QString("Name: ") + get_nlist_name(i),
			QString()));
	}

	cached_actions = result;

	return std::move(result);
}

class QIDACommandPaletteInner : public QPaletteInner {
public:
	QIDACommandPaletteInner(QWidget* parent, const QVector<Action> &items)
		: QPaletteInner(parent, std::move(items)) {
	}

	EnterResult enter_callback(Action &action) override {
		processEnterResult(true);

		auto* model = entries_->model();
		auto id = action.id();
		g_last_used[id] = QDate::currentDate();
		if (id.startsWith("@ ")) {
			auto address = id.mid(2).toUInt(nullptr, 16);
			jumpto(address);
		}
		else
			process_ui_action(id.toStdString().c_str());
		// already hidden
		return false;
	}
};

class enter_handler : public action_handler_t {
	int idaapi activate(action_activation_ctx_t*) override {
		show_palette(new QIDACommandPaletteInner(nullptr, std::move(getActions())));

		return 1;
	}

	action_state_t idaapi update(action_update_ctx_t*) override {
		return AST_ENABLE_ALWAYS;
	}
};

extern plugin_t PLUGIN;

static enter_handler enter_handler_;

static action_desc_t enter_action = ACTION_DESC_LITERAL(
	"ifred:enter",
	"ifred command palette",
	&enter_handler_,
	"Ctrl+Shift+P",
	"command palette",
	-1);

//--------------------------------------------------------------------------
bool idaapi run(size_t) {
	return true;
}

extern char comment[], help[], wanted_name[], wanted_hotkey[];

//--------------------------------------------------------------------------
char comment[] = "ifred";

char help[] =
"IDA palette";

//--------------------------------------------------------------------------
// This is the preferred name of the plugin module in the menu system
// The preferred name may be overriden in plugins.cfg file

char wanted_name[] = "ifred";

void init_python_module();

ssize_t idaapi load_python(void* user_data, int notification_code, va_list va) {
	auto info = va_arg(va, plugin_info_t *);

	if (notification_code == ui_plugin_loaded && !strcmp(info->org_name, "IDAPython")) {
		{
			pybind11::gil_scoped_acquire gil;
			init_python_module();
		}

		unhook_from_notification_point(HT_UI, load_python, NULL);
	}

	return 0;
}

const QString IdaPluginPath(const char* filename) {
	static QString g_plugin_path;
	if (g_plugin_path.size()) {
		QString r = g_plugin_path + filename;
		qDebug() << r;
		return r;
	}

	g_plugin_path = (QString(get_user_idadir()).replace("\\", "/") + QString("/plugins/palette/"));
	QDir plugin_dir(g_plugin_path);
	plugin_dir.mkpath(".");

	return g_plugin_path + filename;
}

//--------------------------------------------------------------------------
int idaapi init(void) {
	// the plugin works only with idaq
	int r = is_idaq() ? PLUGIN_KEEP : PLUGIN_SKIP;
	if (r == PLUGIN_KEEP) {
		msg("ifred loading...\n");

		// init libpalette
		if (!Py_IsInitialized())
			hook_to_notification_point(HT_UI, load_python, NULL);
		else
			init_python_module();

		set_path_handler(IdaPluginPath);
		// init libpalette complete

		getActions();

		if (!register_action(enter_action)) {
			msg("ifred action loading error");
			return PLUGIN_SKIP;
		};

		update_action_shortcut("CommandPalette", "");
	}
	return r;
}

//--------------------------------------------------------------------------
void idaapi term(void) {
	cleanup_palettes();
}

//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
		IDP_INTERFACE_VERSION,
		PLUGIN_FIX | PLUGIN_HIDE, // plugin flags
		init,                     // initialize

		term, // terminate. this pointer may be NULL.

		run, // invoke plugin

		comment, // long comment about the plugin
		// it could appear in the status line
		// or as a hint

		help, // multiline help about the plugin

		wanted_name, // the preferred short name of the plugin
		""           // wanted_hotkey; the preferred hotkey to run the plugin
};