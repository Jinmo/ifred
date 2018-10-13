/*
 *  This is a sample plugin module. It demonstrates how to fully use
 *  the Qt environment in IDA.
 *
 */

#include <QtGui>
#include <QtWidgets>

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

// include your own widget here
#include "common_defs.h"
#include "myfilter.h"
#include "qsearch.h"
#include "qcommands.h"

static QSet<QString> g_is_interned;

QHash<ushort, QSet<QString>> g_intern;
QSet<QString> g_search;
bool highlightTable[65536]; // not emoji!

inline void CenterWidgets(QWidget *widget, QWidget *host = nullptr) {
    if (!host)
        host = widget->parentWidget();

    if (host) {
        auto hostRect = host->geometry();
        widget->move(hostRect.center() - widget->rect().center());
    }
    else {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }
}

#define WIDTH 720
#define HEIGHT 74

class QItem: public QStyledItemDelegate {
public:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        auto model = index.model();
        QTextDocument doc;
        doc.setDefaultStyleSheet(R"(
                                 div {
                                 font-family: Segoe UI;
                                 font-size: 18px;
                                 display: block;
                                 color: #959DA6;
                                 }

                                 span {
                                    font-weight: bold;
                                 color: #F2973D;
                                 }
                                 )");
        QString tooltip = model->data(model->index(index.row(), 0)).toString();
        QString highlighted;

        highlighted.reserve(tooltip.size() * 6);
        // char escaped[100];
        bool hl_state = false;
        for(auto &i: tooltip) {
            // QT::qsnprintf(escaped, sizeof(escaped), "&#x%04hx;", i.unicode());
            if(highlightTable[i.toLower().unicode()]) {
                if(!hl_state) {
                    highlighted = highlighted % "<span>" % QString(i);
                    hl_state = true;
                }
                else {
                    highlighted = highlighted % QString(i);
                }
            } else {
                if(!hl_state)
                    highlighted = highlighted % QString(i);
                else {
                    highlighted = highlighted % "</span>" % QString(i);
                    hl_state = false;
                }
            }
        }
//        msg("%s %08x\n", highlighted.toStdString().c_str(), option.state);
        doc.setHtml(QString("<div>") % highlighted % "</div>");

        painter->save();

        QStyleOptionViewItem newOption = option;
        newOption.state = option.state & (~QStyle::State_HasFocus);

//        newOption.widget->style()->drawControl(QStyle::CE_ItemViewItem, &newOption, painter);
        painter->translate(option.rect.left(), option.rect.top());

        if(option.state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
            painter->fillRect(0, 0, option.rect.width(), option.rect.height(), QBrush("#f5f5f5"));
        }

        painter->translate(28, 8);

        doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex &) const override {
        return QSize(0, 48);
    }
};

struct Action {
    QString id, tooltip, shortcut;
    Action(qstring id_, qstring tooltip_, qstring shortcut_): id(QString(id_.c_str())), tooltip(QString(tooltip_.c_str())), shortcut(QString(shortcut_.c_str())) {}
};


void internString(QString &src) {
    if(g_is_interned.contains(src))
        return;
    g_is_interned.insert(src);
    for(auto &i: src) {
        auto c = i.toLower().unicode();
        auto &set = g_intern[c];
        set.insert(src);
    }
}

class Qfred: public QFrame {

    QMainWindow *mainWindow_;

    QVBoxLayout layout_;
    QSearch searchbox_;

    QCommands commands_;
    MyFilter proxy;

    QStandardItemModel model;
public:
    auto &searchbox() { return searchbox_; }
    Qfred(QMainWindow *parent): mainWindow_(nullptr), searchbox_(this, &proxy), model(0, 2) {
        setWindowFlags( Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        setStyleSheet(R"(
                      font-family: Segoe UI;
                      background: #fff;
                      QFrame {
                        border-radius: 7px;
                      }
                      )");

        proxy.setDynamicSortFilter(true);
        proxy.setSourceModel(&model);

        populateList();

        // TODO: we need to repaint the list item... I don't know how.
        connect(&proxy, &MyFilter::dataChanged, &commands_, [=](){ commands_.viewport()->update(); });
        commands_.setModel(&proxy);
        commands_.setItemDelegate(new QItem());
        commands_.setLineWidth(0);
        commands_.setStyleSheet(R"(
                                QListView {border: none;}
                                QScrollBar:vertical {
                                border: none;
                                background: #f5f5f5;
                                width: 12px;
                                border-radius: 6px;
                                }
                                QScrollBar::handle:vertical {
                                background: #dadada;
                                min-height: 20px;
                                border-radius: 6px;
                                }
                                QScrollBar::add-line:vertical {
                                width: 0; height: 0;
                                subcontrol-position: bottom;
                                subcontrol-origin: margin;
                                }
                                QScrollBar::sub-line:vertical {
                                width: 0; height: 0;
                                subcontrol-position: top;
                                subcontrol-origin: margin;
                                }
                                )");

        layout_.addWidget(&searchbox_);
        layout_.addWidget(&commands_);
        layout_.setContentsMargins(0, 0, 0, 0);
        layout_.setSpacing(0);

        setLayout(&layout_);

        resize(WIDTH, HEIGHT);

        parent->installEventFilter(this);

        connect(&searchbox_, &QSearch::returnPressed, this, &Qfred::enter_callback);
        connect(&searchbox_, &QSearch::textChanged, this, &Qfred::onTextChanged);
    }

    void onTextChanged(const QString &text) {
        commands_.setCurrentIndex(proxy.index(0,0));
        commands_.scrollToTop();
    }

    bool arrow_callback(int key) {
        int delta;
        if(key == Qt::Key_Down) {
            delta = 1;
        } else {
            delta = -1;
        }
        auto new_row = commands_.currentIndex().row() + delta;
        if(new_row == -1) new_row = 0;
        commands_.setCurrentIndex(proxy.index(new_row, 0));
        return true;
    }

    bool enter_callback() {
        auto id = model.data(model.index(proxy.mapToSource(commands_.currentIndex()).row(), 2)).toString();
        mainWindow_->hide();
        process_ui_action(id.toStdString().c_str());
        return true;
    }

    bool eventFilter(QObject *obj, QEvent *event) override {
        // if(obj != &searchbox_ && obj != &commands_) return QFrame::eventFilter(obj, event);
        switch(event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            switch (ke->key()) {
            case Qt::Key_Down:
            case Qt::Key_Up:
                return arrow_callback(ke->key());
            default:
                return QFrame::eventFilter(obj, event);
            }
        }
        default:
            return QFrame::eventFilter(obj, event);
        }
    }


    qvector<Action *> *getActions() {
        qstrvec_t id_list;
        qvector<Action *> *result = new qvector<Action *>();

        get_registered_actions(&id_list);

        result->reserve(id_list.size());
        for(auto &item: id_list) {
            qstring tooltip, shortcut;
            get_action_tooltip(&tooltip, item.c_str());
            get_action_shortcut(&shortcut, item.c_str());
            result->push_back(new Action(item, tooltip, shortcut));
        }

        return result;
    }

    void populateList() {
        // TODO: cache it by id
        auto out = getActions();

        model.setRowCount(static_cast<int>(out->size()));
        model.setColumnCount(3);

        int i = 0;
        for(auto &item: *out) {
            model.setData(model.index(i, 0), item->tooltip);
            model.setData(model.index(i, 1), item->shortcut);
            model.setData(model.index(i, 2), item->id);

            internString(item->tooltip);
            i += 1;
        }
    }

    void keyPressEvent(QKeyEvent *e) override {
        if(e->key() != Qt::Key_Escape)
            QFrame::keyPressEvent(e);
        else {
            if(mainWindow_) mainWindow_->hide();
        }
    }

    void setMainWindow(QMainWindow *newWindow) {
        mainWindow_ = newWindow;
    }
};

class QfredContainer: public QMainWindow {
    Qfred fred_;
public:
    auto &fred() { return fred_; }
    QfredContainer(): fred_(this) {
        const int kShadow = 30;
        setWindowFlags( Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(kShadow);
        effect->setColor(QColor(0, 0, 0, 100));
        effect->setOffset(0);
        fred_.setGraphicsEffect(effect);
        setCentralWidget(&fred_);

        fred_.setMainWindow(this);

        setContentsMargins(kShadow, kShadow, kShadow, kShadow);
        CenterWidgets(this);

    }
    void focus() {
        fred_.searchbox().selectAll();
        fred_.searchbox().setFocus();
    }
};

static QfredContainer widget;
int first_execution = 1;

class enter_handler: public action_handler_t {
    int idaapi activate(action_activation_ctx_t *ctx) override {
        if(first_execution) {
            auto *ida_twidget = create_empty_widget("ifred");
            QWidget *ida_widget = reinterpret_cast<QWidget *>(ida_twidget);
            ida_widget->setGeometry(0, 0, 0, 0);
            ida_widget->setWindowFlags( Qt::FramelessWindowHint);
            ida_widget->setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent
            display_widget(ida_twidget, 0);
            widget.setParent(ida_widget->window()->parentWidget()->window());
            close_widget(ida_twidget, 0);
            widget.fred().populateList();
        }

        first_execution = 0;
        widget.show();
        widget.focus();

        return 1;
    }
    action_state_t idaapi update(action_update_ctx_t *ctx) override {
        return AST_ENABLE_ALWAYS;
    }
};

static enter_handler enter_handler_;
static action_desc_t enter_action = ACTION_DESC_LITERAL(
            "ifred:enter",
            "ifred command palette",
            &enter_handler_,
            "Ctrl+Shift+P",
            "command palette",
            -1
            );

//--------------------------------------------------------------------------
bool idaapi run(size_t)
{
  return true;
}

//--------------------------------------------------------------------------
static ssize_t idaapi ui_callback(void *user_data, int notification_code, va_list va)
{
  if ( notification_code == ui_widget_visible )
  {
    TWidget *widget = va_arg(va, TWidget *);
    if ( widget == user_data )
    {
    }
  }
  if ( notification_code == ui_widget_invisible )
  {
    TWidget *widget = va_arg(va, TWidget *);
    if ( widget == user_data )
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
  if(r == PLUGIN_KEEP) {
        hook_to_notification_point(HT_UI, &ui_callback, &widget);

        msg("ifred loading...\n");

        if(!register_action(enter_action)) {
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
  PLUGIN_FIX | PLUGIN_HIDE,                    // plugin flags
  init,                 // initialize

  term,                 // terminate. this pointer may be NULL.

  run,                  // invoke plugin

  comment,              // long comment about the plugin
  // it could appear in the status line
  // or as a hint

  help,                 // multiline help about the plugin

  wanted_name,          // the preferred short name of the plugin
  wanted_hotkey         // the preferred hotkey to run the plugin
};
