#include "plugin.h"
#include <Python.h>

#include <QtGui>
#include <QtWidgets>

#include <palette_api.h>
#include <utils.h>

#ifndef __APPLE__
#define CMD_PALETTE_SHORTCUT "Ctrl+Shift+P"
#define NAME_PALETTE_SHORTCUT "Ctrl+P"
#else
#define CMD_PALETTE_SHORTCUT "Meta+Shift+P"
#define NAME_PALETTE_SHORTCUT "Meta+P"
#endif

#ifdef __MAC__
#include <dlfcn.h>
bool mac_dlopen_workaround()
{
    Dl_info res;
    if (dladdr(&PLUGIN, &res) == 0)
    {
        return false;
    }

    auto res_open = dlopen(res.dli_fname, RTLD_NODELETE);
    if (!res_open)
    {
        return false;
    }

    return true;
}
#endif

QVector<QRegularExpression> getBlacklist()
{
    auto blacklist = json("config.json")["blacklist"].toArray();
    QVector<QRegularExpression> blacklist_converted;

    for (auto&& i : blacklist)
    {
        if (i.toString().size())
            blacklist_converted.push_back(QRegularExpression(i.toString()));
    }
    return blacklist_converted;
}

void addActions(QVector<Action>& result, const qstrvec_t& actions)
{
    qstring tooltip, shortcut;
    action_state_t state;

    auto blacklist = getBlacklist();

    QRegularExpression remove_tilde("~(.*?)~");

    for (auto& item : actions)
    {
        // Check blacklist
        bool skip = false;
        for (auto& pattern : blacklist)
            if (pattern.match(item.c_str()).hasMatch())
            {
                skip = true;
                break;
            }

        if (skip)
            continue;

        // Enabled actions only
        if (!get_action_state(item.c_str(), &state))
            continue;

        if (state > AST_ENABLE)
            continue;

        // Get metadata for the action
        QString tooltip_qstr;

        get_action_label(&tooltip, item.c_str());
        get_action_shortcut(&shortcut, item.c_str());

        tooltip_qstr = QString::fromUtf8(tooltip.c_str());
        tooltip_qstr = tooltip_qstr.replace(remove_tilde, "\\1");

        result.push_back(Action{ QString(item.c_str()), tooltip_qstr, QString(shortcut.c_str()) });
    }
}

void addNames(QVector<Action>& result, size_t names)
{
    for (size_t i = 0; i < names; i++)
    {
        const char* name = get_nlist_name(i);
        qstring demangled_name = demangle_name(name, 0);

        result.push_back(Action{ QString::number(get_nlist_ea(i), 16) + ":" + name,
                                (demangled_name.empty() ? name : demangled_name.c_str()),
                                QString() });
    }
}

// Get command palette items from IDA: 1. actions, 2. names
const QVector<Action> getActions()
{
    QVector<Action> result;

    qstrvec_t actions;
    get_registered_actions(&actions);

    // 0. Reserve vector to avoid multiple allocations
    result.reserve(static_cast<int>(actions.size()));

    // 1. Add actions from IDA except blacklisted identifiers
    addActions(result, actions);

    std::sort(result.begin(), result.begin() + result.size(), [](Action& lhs, Action& rhs) {
        return lhs.name.compare(rhs.name) < 0;
        });

    return result;
}

void addStructs(QVector<Action>& result)
{
    int idx = get_first_struc_idx();
    while (idx != BADADDR)
    {
        tid_t sid = get_struc_by_idx(idx);
        result.push_back(Action{ "struct:" + QString::number(sid),
			QString::fromStdString(get_struc_name(sid).c_str()), QString() });

        idx = get_next_struc_idx(idx);
    }
}

class NamesManager {
    QHash<ea_t, Action*> address_to_name;
    QHash<tid_t, Action*> address_to_struct;
    QVector<Action> result;
public:
    NamesManager() {
        hook_to_notification_point(HT_IDB, idb_hooks, this);
        hook_to_notification_point(HT_IDP, idp_hooks, this);
    }

    void init(QVector<Action>* names) {
        address_to_name.clear();
        address_to_struct.clear();

        for (auto& action : *names) {
            auto sep = action.id.indexOf(':');
            if (action.id.startsWith("struct:")) {
                address_to_struct.insert(
                    action.id.midRef(sep + 1).toULongLong(nullptr), &action);
            }
            else {
                address_to_name.insert(
                    action.id.midRef(0, sep).toULongLong(nullptr, 16), &action);
            }
        }
    }

    void rename(ea_t address, const char* name) {
        auto it = address_to_name.find(address);
		qstring demangled_name = demangle_name(name, 0);

		if (it == address_to_name.end()) {
			result.push_back(Action{ QString::number(address, 16) + ":" + name,
									(demangled_name.empty() ? name : demangled_name.c_str()),
									QString() });
			address_to_name.insert(address, &result.back());
			return;
		}

        Action* action = it.value();
        action->name = QString::fromStdString((demangled_name.empty() ? name : demangled_name.c_str()));
        action->id = QString::number(address, 16) + ":" + name;
    }

    void rebase(segm_move_infos_t& infos) {
        std::vector<std::pair<ea_t, ea_t>> moves;
        for (auto&& seg : infos) {
            for (auto&& key : address_to_name.keys()) {
                if (key >= seg.from && key < seg.from + seg.size) {
                    moves.push_back({key, key + seg.to - seg.from});
                }
            }
        }
        for (auto &&move: moves) {
            auto value = address_to_name[move.first];
            address_to_name.remove(move.first);
            value->id = QString::number(move.second, 16) + ":" + value->name;
            address_to_name[move.second] = value;
        }
    }

    void struc_rename(tid_t id, const char* name) {
        auto it = address_to_struct.find(id);
		if (it == address_to_struct.end()) {
			result.push_back(Action{ "struct:" + id, name, QString() });
			address_to_struct.insert(id, &result.back());
		}

        Action* action = it.value();
        action->name = QString::fromStdString(name);
    }

    void clear() {
        result.clear();
        address_to_name.clear();
        address_to_struct.clear();
    }

    QVector<Action> get(bool clear = false) {
        size_t names = get_nlist_size();
        size_t structs = get_struc_qty();

        if (result.size() != 0 && !clear)
            return result;

        // 0. Reserve vector to avoid multiple allocations
        result.reserve(names + structs);

        // 1. Add names from IDA
        addNames(result, names);

        // 2. Add structs from IDA
        addStructs(result);

        init(&result);

        return result;
    }

    static ssize_t idaapi idb_hooks(void* user_data, int notification_code, va_list va) {
        auto manager = reinterpret_cast<NamesManager*>(user_data);

        switch (notification_code) {
        case idb_event::allsegs_moved: {
            auto info = va_arg(va, segm_move_infos_t*);
            manager->rebase(*info);
            break;
        }
        case idb_event::renamed:
        {
            auto ea = va_arg(va, ea_t);
            auto new_name = va_arg(va, const char*);
            manager->rename(ea, new_name);
            break;
        }
        case idb_event::struc_renamed:
        {
            auto struc = va_arg(va, struc_t*);
            if (struc)
                manager->struc_rename(
                    struc->id, get_struc_name(struc->id).c_str());
        }
        }
        return 0;
    }

    static ssize_t idaapi idp_hooks(void* user_data, int notification_code, va_list va) {
        auto manager = reinterpret_cast<NamesManager*>(user_data);

        switch (notification_code) {
        case processor_t::ev_term:
            manager->clear();
        }
        return 0;
    }

};

const QVector<Action> getNames(bool clear = false)
{
    static NamesManager* manager;

    if (!manager) {
        manager = new NamesManager();
    }

    return manager->get();
}

void init_rename_hooks() {
}

class command_palette_handler : public action_handler_t
{
    int idaapi activate(action_activation_ctx_t* context) override
    {
        qstring shortcut;
        get_action_shortcut(&shortcut, context->action);
        shortcut.replace("-", "+");
        show_palette("command palette", "Enter action or option name...", getActions(), shortcut.c_str(), [](Action& action) {
            process_ui_action(action.id.toStdString().c_str());
            return true;
            });
        return 1;
    }

    action_state_t idaapi update(action_update_ctx_t*) override
    {
        return AST_ENABLE_ALWAYS;
    }
};

class name_palette_handler : public action_handler_t
{
    int idaapi activate(action_activation_ctx_t* context) override
    {
        qstring shortcut;
        get_action_shortcut(&shortcut, context->action);
        shortcut.replace("-", "+");
        show_palette("name palette" + QString(get_path(PATH_TYPE_IDB)), "Enter symbol name...", getNames(), shortcut.c_str(), [](Action& action) {
            auto&& id = action.id;

            if (id.startsWith("struct:"))
            {
                open_structs_window(id.midRef(7).toULongLong());
            }
            else
            {
                int sep = action.id.indexOf(':');
                ea_t address = static_cast<ea_t>(id.midRef(0, sep).toULongLong(nullptr, 16));
                jumpto(address);
                reg_update_strlist("History\\$", action.id.midRef(sep + 1).toUtf8().data(), 32);
            }

            return true;
            });
        return 1;
    }

    action_state_t idaapi update(action_update_ctx_t*) override
    {
        return AST_ENABLE_ALWAYS;
    }
};

extern plugin_t PLUGIN;

static command_palette_handler command_palette_handler_;
static name_palette_handler name_palette_handler_;

static action_desc_t command_palette_action = ACTION_DESC_LITERAL(
    "ifred:command_palette",
    "ifred command palette",
    &command_palette_handler_,
    CMD_PALETTE_SHORTCUT,
    "command palette",
    -1);

static action_desc_t name_palette_action = ACTION_DESC_LITERAL(
    "ifred:name_palette",
    "ifred name palette",
    &name_palette_handler_,
    NAME_PALETTE_SHORTCUT,
    "name palette",
    -1);

//--------------------------------------------------------------------------
bool idaapi run(size_t)
{
    return true;
}

extern char comment[], help[], wanted_name[];

//--------------------------------------------------------------------------
char comment[] = "ifred";

char help[] = "IDA palette";

//--------------------------------------------------------------------------
// This is the preferred name of the plugin module in the menu system
// The preferred name may be overridden in plugins.cfg file

char wanted_name[] = "ifred";

void init_python_module();

class gil_scoped_acquire
{
    PyGILState_STATE state;
    bool reset;

public:
    gil_scoped_acquire()
    {
        reset = false;
        state = PyGILState_Ensure();
    }

    ~gil_scoped_acquire()
    {
        if (!reset)
            PyGILState_Release(state);
    }
};

void initpy()
{
    postToMainThread([]() {
        gil_scoped_acquire gil;
        init_python_module();
        });
}

ssize_t idaapi load_python(void*, int notification_code, va_list va)
{
    auto info = va_arg(va, plugin_info_t*);

    if (notification_code == ui_plugin_loaded && !strcmp(info->org_name, "IDAPython"))
    {
        initpy();

        unhook_from_notification_point(HT_UI, load_python, nullptr);
    }

    return 0;
}

const QString IdaPluginPath(const char* filename)
{
    static QString g_plugin_path;
    if (g_plugin_path.size())
    {
        QString r = g_plugin_path + filename;
        return r;
    }

    g_plugin_path = (QString(get_user_idadir()).replace("\\", "/") + QStringLiteral("/plugins/palette/"));
    QDir plugin_dir(g_plugin_path);
    plugin_dir.mkpath(".");

    return g_plugin_path + filename;
}

//--------------------------------------------------------------------------
int idaapi init()
{
    if (!is_idaq())
        // the plugin works only with idaq
        return PLUGIN_SKIP;

    msg("loading palettes...\n");

    // 1. init IDAPython
    if (!Py_IsInitialized())
        hook_to_notification_point(HT_UI, load_python, nullptr);
    else
        initpy();

    // 2. init theme path handler
    set_path_handler(IdaPluginPath);

    if (!register_action(command_palette_action))
    {
        msg("command palette action loading error\n");
        return PLUGIN_SKIP;
    };

    if (!register_action(name_palette_action))
    {
        msg("name palette action loading error\n");
        return PLUGIN_SKIP;
    };

    init_rename_hooks();

#ifdef __MAC__
    if (!mac_dlopen_workaround())
    {
        msg("ifred mac dlopen workaround error\n");
        return PLUGIN_SKIP;
}
#endif

#ifdef _WIN32
    HMODULE hModule;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)& init, &hModule))
    {
        msg("ifred windows loadlibrary workaround error");
        return PLUGIN_SKIP;
    }
#endif

    qstring shortcut, shortcut2;
    get_action_shortcut(&shortcut, "CommandPalette");
    get_action_shortcut(&shortcut2, command_palette_action.name);

    if (shortcut == "Ctrl-Shift-P" && shortcut == shortcut2)
        update_action_shortcut("CommandPalette", "");

    return PLUGIN_KEEP;
}

//--------------------------------------------------------------------------
void idaapi term()
{
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
    ""           // the preferred hotkey to run the plugin
};
