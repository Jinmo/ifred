/*
 *  This is a sample plugin module. It demonstrates how to fully use
 *  the Qt environment in IDA.
 *
 */

#include "ida_headers.h"

#include <QtGui>
#include <QtWidgets>

#include "common_defs.h"
#include "myfilter.h"
#include "qsearch.h"
#include "qitems.h"
#include "qpalette_.h"

QSet<QString> g_is_interned;
QHash<ushort, QSet<QString>> g_intern;

void internString(QString &src)
{
    if (g_is_interned.contains(src))
        return;
    g_is_interned.insert(src);
    for (auto &i : src)
    {
        auto c = i.toLower().unicode();
        auto &set = g_intern[c];
        set.insert(src);
    }
}

struct Action
{
    QString id, tooltip, shortcut;
    Action(qstring id_, qstring tooltip_, qstring shortcut_)
        : id(QString(id_.c_str())),
          tooltip(QString(tooltip_.c_str())),
          shortcut(QString(shortcut_.c_str())) {}
};

class QIDACommandPaletteInner : public QPaletteInner
{
  public:
    QIDACommandPaletteInner() : QPaletteInner()
    {
        populateList();
    }

    EnterResult enter_callback() override
    {
        processEnterResult(true);

        auto &model = entries_.model();
        auto id = model.data(model.index(entries_.currentIndex().row(), 2)).toString();
        g_last_used[id] = QDate::currentDate();
        process_ui_action(id.toStdString().c_str());
        // already hidden
        return false;
    }

    std::vector<Action *> *getActions()
    {
        qstrvec_t id_list;
        auto *result = new std::vector<Action *>();

        get_registered_actions(&id_list);

        result->reserve(id_list.size());
        for (auto &item : id_list)
        {
            qstring tooltip, shortcut;
            get_action_tooltip(&tooltip, item.c_str());
            get_action_shortcut(&shortcut, item.c_str());
            result->push_back(new Action(item, tooltip, shortcut));
        }

        return result;
    }

    void populateList()
    {
        // TODO: cache it by id
        auto out = getActions();
        auto &source = entries_.source();

        source.setRowCount(static_cast<int>(out->size()));
        source.setColumnCount(3);

        int i = 0;
        for (auto &item : *out)
        {
            source.setData(source.index(i, 0), item->tooltip);
            source.setData(source.index(i, 1), item->shortcut);
            source.setData(source.index(i, 2), item->id);

            internString(item->tooltip);
            i += 1;
        }
    }
};

QPalette_<QIDACommandPaletteInner> widget;
QHash<QString, QDate> g_last_used;

int first_execution = 1;

class enter_handler : public action_handler_t
{
    int idaapi activate(action_activation_ctx_t *ctx) override
    {
        if (first_execution)
        {
            auto *ida_twidget = create_empty_widget("ifred");
            QWidget *ida_widget = reinterpret_cast<QWidget *>(ida_twidget);

            ida_widget->setGeometry(0, 0, 0, 0);
            ida_widget->setWindowFlags(Qt::FramelessWindowHint);
            ida_widget->setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

            display_widget(ida_twidget, 0);
            widget.setParent(ida_widget->window()->parentWidget()->window());
            close_widget(ida_twidget, 0);
        }

        first_execution = 0;
        widget.show();
        widget.focus();

        return 1;
    }
    action_state_t idaapi update(action_update_ctx_t *ctx) override
    {
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
bool idaapi run(size_t)
{
    return true;
}

//--------------------------------------------------------------------------
static ssize_t idaapi ui_callback(void *user_data, int notification_code, va_list va)
{
    if (notification_code == ui_widget_visible)
    {
        TWidget *widget = va_arg(va, TWidget *);
        if (widget == user_data)
        {
        }
    }
    if (notification_code == ui_widget_invisible)
    {
        TWidget *widget = va_arg(va, TWidget *);
        if (widget == user_data)
        {
            // widget is closed, destroy objects (if required)
        }
    }
    return 0;
}

extern char comment[], help[], wanted_name[], wanted_hotkey[];

//--------------------------------------------------------------------------
char comment[] = "ifred pm";

char help[] =
    "IDA package manager";

//--------------------------------------------------------------------------
// This is the preferred name of the plugin module in the menu system
// The preferred name may be overriden in plugins.cfg file

char wanted_name[] = "ifred";

// This is the preferred hotkey for the plugin module
// The preferred hotkey may be overriden in plugins.cfg file
// Note: IDA won't tell you if the hotkey is not correct
//       It will just disable the hotkey.

char wanted_hotkey[] = "";

//--------------------------------------------------------------------------
int idaapi init(void)
{
    // the plugin works only with idaq
    int r = is_idaq() ? PLUGIN_KEEP : PLUGIN_SKIP;
    if (r == PLUGIN_KEEP)
    {
        hook_to_notification_point(HT_UI, &ui_callback, &widget);

        msg("ifred loading...\n");

        if (!register_action(enter_action))
        {
            msg("ifred action loading error");
        };
    }
    return r;
}

//--------------------------------------------------------------------------
void idaapi term(void)
{
    unhook_from_notification_point(HT_UI, ui_callback);
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

        wanted_name,  // the preferred short name of the plugin
        wanted_hotkey // the preferred hotkey to run the plugin
};