#include "plugin.h"
#include <Python.h>

#include <QtGui>
#include <QtWidgets>

#include <widgets/palette_manager.h>
#include <utils.h>

#ifdef __MAC__
#include <dlfcn.h>
bool mac_dlopen_workaround() {
    Dl_info res;
    if (dladdr(&PLUGIN, &res) == 0) {
        return false;
    }

    auto res_open = dlopen(res.dli_fname, RTLD_NODELETE);
    if (!res_open) {
        return false;
    }

    return true;
}
#endif

QVector<QRegularExpression> getBlacklist() {
    auto blacklist = json("config.json")["blacklist"].toArray();
    QVector<QRegularExpression> blacklist_converted;

    for (auto&& i : blacklist) {
        if (i.toString().size())
            blacklist_converted.push_back(QRegularExpression(i.toString()));
    }
    return blacklist_converted;
}

void addActions(QVector<Action>& result, const qstrvec_t& actions) {
    qstring tooltip, shortcut;
    action_state_t state;

    auto blacklist = getBlacklist();

    for (auto& item : actions) {
        // Check blacklist
        bool skip = false;
        for (auto& pattern : blacklist)
            if (pattern.match(item.c_str()).hasMatch()) {
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
        get_action_tooltip(&tooltip, item.c_str());
        get_action_shortcut(&shortcut, item.c_str());
        result.push_back(Action(
            QString(item.c_str()), QString(tooltip.c_str()), QString(shortcut.c_str())));
    }
}

void addNames(QVector<Action>& result, size_t names) {
    for (size_t i = 0; i < names; i++) {
        const char* name = get_nlist_name(i);
        qstring demangled_name = demangle_name(name, 0);

        result.push_back(Action(
            QString::number(get_nlist_ea(i), 16).toUpper(),
            (demangled_name.empty() ? name : demangled_name.c_str()),
            QString()));
    }
}

// Get command palette items from IDA: 1. actions, 2. names
const QVector<Action> getActions() {
    QVector<Action> result;

    qstrvec_t actions;
    get_registered_actions(&actions);

    // 0. Reserve vector to avoid multiple allocations
    result.reserve(static_cast<int>(actions.size()));

    // 1. Add actions from IDA except blacklisted identifiers
    addActions(result, actions);

    std::sort(result.begin(), result.begin() + result.size(), [](Action & lhs, Action & rhs) {
        return lhs.description().compare(rhs.description()) < 0;
    });

    return result;
}

const QVector<Action> getNames() {
    QVector<Action> result;
    size_t names = get_nlist_size();

    // 0. Reserve vector to avoid multiple allocations
    result.reserve(names);

    // 1. Add names from IDA
    addNames(result, names);

    return result;
}

class command_palette_handler : public action_handler_t {
    int idaapi activate(action_activation_ctx_t*) override {
        show_palette("command palette", "Enter action or option name...", getActions(), [](Action & action) {
            auto &&id = action.id();

            process_ui_action(id.toStdString().c_str());
            return true;
            });
        return 1;
    }

    action_state_t idaapi update(action_update_ctx_t*) override {
        return AST_ENABLE_ALWAYS;
    }
};

class name_palette_handler : public action_handler_t {
    int idaapi activate(action_activation_ctx_t*) override {
        show_palette("name palette", "Enter symbol name...", getNames(), [](Action & action) {
            auto &&id = action.id();

            ea_t address = static_cast<ea_t>(id.toULongLong(nullptr, 16));
            jumpto(address);

            return true;
            });
        return 1;
    }

    action_state_t idaapi update(action_update_ctx_t*) override {
        return AST_ENABLE_ALWAYS;
    }
};

extern plugin_t PLUGIN;

static command_palette_handler command_palette_handler_;
static name_palette_handler name_palette_handler_;

#ifndef __APPLE__
#define CMD_PALETTE_SHORTCUT "Ctrl+Shift+P"
#define NAME_PALETTE_SHORTCUT "Ctrl+P"
#else
#define CMD_PALETTE_SHORTCUT "Meta+Shift+P"
#define NAME_PALETTE_SHORTCUT "Meta+P"
#endif
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
bool idaapi run(size_t) {
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

class gil_scoped_acquire {
    PyGILState_STATE state;
    bool reset;
public:
    gil_scoped_acquire() { reset = false; state = PyGILState_Ensure(); }
    ~gil_scoped_acquire() { if (!reset) PyGILState_Release(state); }
};

void initpy() {
    postToMainThread([]() {
        gil_scoped_acquire gil;
        init_python_module();
        });
}

ssize_t idaapi load_python(void*, int notification_code, va_list va) {
    auto info = va_arg(va, plugin_info_t*);

    if (notification_code == ui_plugin_loaded && !strcmp(info->org_name, "IDAPython")) {
        initpy();

        unhook_from_notification_point(HT_UI, load_python, nullptr);
    }

    return 0;
}

const QString IdaPluginPath(const char* filename) {
    static QString g_plugin_path;
    if (g_plugin_path.size()) {
        QString r = g_plugin_path + filename;
        return r;
    }

    g_plugin_path = (QString(get_user_idadir()).replace("\\", "/") + QStringLiteral("/plugins/palette/"));
    QDir plugin_dir(g_plugin_path);
    plugin_dir.mkpath(".");

    return g_plugin_path + filename;
}

//--------------------------------------------------------------------------
int idaapi init() {
    if(!is_idaq())
        // the plugin works only with idaq
        return PLUGIN_SKIP;

    msg("ifred loading...\n");

    // 1. init IDAPython
    if (!Py_IsInitialized())
        hook_to_notification_point(HT_UI, load_python, nullptr);
    else
        initpy();

    // 2. init theme path handler
    set_path_handler(IdaPluginPath);

    if (!register_action(command_palette_action)) {
        msg("ifred command palette action loading error");
        return PLUGIN_SKIP;
    };

    if (!register_action(name_palette_action)) {
        msg("ifred name palette action loading error");
        return PLUGIN_SKIP;
    };

#ifdef __MAC__
    if (!mac_dlopen_workaround()) {
        msg("ifred mac dlopen workaround error");
        return PLUGIN_SKIP;
    }
#endif

#ifdef _WIN32
    HMODULE hModule;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)& init, &hModule)) {
        msg("ifred windows loadlibrary workaround error");
        return PLUGIN_SKIP;
    }
#endif

    update_action_shortcut("CommandPalette", "");
    return PLUGIN_KEEP;
}

//--------------------------------------------------------------------------
void idaapi term() {
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
