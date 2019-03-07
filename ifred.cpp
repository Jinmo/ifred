/*
 *  This is a sample plugin module. It demonstrates how to fully use
 *  the Qt environment in IDA.
 *
 */

#include "ida_headers.h"

#include <QtGui>
#include <QtWidgets>

#include "common_defs.h"
#include "qpalette_.h"

QHash<QString, QDate> g_last_used;

struct Action
{
    QString id, tooltip, shortcut;
    Action(qstring id_, qstring tooltip_, qstring shortcut_)
        : id(QString(id_.c_str())),
          tooltip(QString(tooltip_.c_str())),
          shortcut(QString(shortcut_.c_str())) {}
};

std::vector<Action *> cached_actions;

QVector<QRegularExpression> getBlacklist()
{
    auto blacklist = config()["blacklist"].toArray();
    QVector<QRegularExpression> blacklist_converted;

    for (int i = 0; i < blacklist.size(); i++)
    {
        if (blacklist[i].toString().size())
            blacklist_converted.push_back(QRegularExpression(blacklist[i].toString()));
    }
    return blacklist_converted;
}

std::vector<Action *> *getActions()
{
    qstrvec_t id_list;
    get_registered_actions(&id_list);

    if (cached_actions.size() == id_list.size())
        return &cached_actions;

    auto *result = new std::vector<Action *>();
    auto blacklist = getBlacklist();

    result->reserve(id_list.size());

    // Variables used in the loop below
    qstring tooltip, shortcut;
    action_state_t state;

    for (auto &item : id_list)
    {
        bool skip = false;
        for (auto &pattern : blacklist)
            if (pattern.match(item.c_str()).hasMatch())
            {
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
        result->push_back(new Action(item, tooltip, shortcut));
    }

    cached_actions = *result;

    return result;
}

class QIDACommandPaletteInner : public QPaletteInner
{
  public:
    QIDACommandPaletteInner(QWidget *parent, QObject *prevItem)
        : QPaletteInner(parent, prevItem)
    {
        populateList();
    }

    EnterResult enter_callback() override
    {
        processEnterResult(true);

        auto *model = entries_->model();
        auto id = model->data(model->index(entries_->currentIndex().row(), 2)).toString();
        g_last_used[id] = QDate::currentDate();
        process_ui_action(id.toStdString().c_str());
        // already hidden
        return false;
    }

    void populateList()
    {
        auto out = getActions();
        auto *source = entries_->source();

        source->setRowCount(static_cast<int>(out->size()));
        source->setColumnCount(3);

        int i = 0;
        for (auto &item : *out)
        {
            source->setData(source->index(i, 0), item->tooltip);
            source->setData(source->index(i, 1), item->shortcut);
            source->setData(source->index(i, 2), item->id);
            i += 1;
        }
    }
};

QPaletteContainer *curWidget;

class enter_handler : public action_handler_t
{
    int idaapi activate(action_activation_ctx_t *) override
    {
        if (!curWidget)
            curWidget = new QPaletteContainer();

        curWidget->clear();
        curWidget->add(new QIDACommandPaletteInner(curWidget, nullptr));
        curWidget->show();
        curWidget->showWidget(0);

        return 1;
    }

    action_state_t idaapi update(action_update_ctx_t *) override
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

extern char comment[], help[], wanted_name[], wanted_hotkey[];

//--------------------------------------------------------------------------
char comment[] = "ifred";

char help[] =
    "IDA palette";

//--------------------------------------------------------------------------
// This is the preferred name of the plugin module in the menu system
// The preferred name may be overriden in plugins.cfg file

char wanted_name[] = "ifred";

//--------------------------------------------------------------------------
int idaapi init(void)
{
    // the plugin works only with idaq
    int r = is_idaq() ? PLUGIN_KEEP : PLUGIN_SKIP;
    if (r == PLUGIN_KEEP)
    {
        msg("ifred loading...\n");
        getActions();

        if (!register_action(enter_action))
        {
            msg("ifred action loading error");
        };

        update_action_shortcut("CommandPalette", "");
    }
    return r;
}

//--------------------------------------------------------------------------
void idaapi term(void)
{
    if (curWidget)
        delete curWidget;
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