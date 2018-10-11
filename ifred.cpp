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

static QHash<ushort, QSet<QString>> g_intern;
static QSet<QString> g_is_interned;
static QSet<QString> g_search;

static bool highlightTable[65536]; // not emoji!

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

//--------------------------------------------------------------------------
int idaapi init(void)
{
  // the plugin works only with idaq
  return is_idaq() ? PLUGIN_OK : PLUGIN_SKIP;
}

//--------------------------------------------------------------------------
void idaapi term(void)
{
  unhook_from_notification_point(HT_UI, ui_callback);
}

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
                                 color: rgb(204, 201, 194);
                                 padding: 14px 12px 14px 12px;
                                 }

                                 span {
                                    color:rgb(229, 184, 96);
                                    font-weight: bold;
                                 }
                                 )");
        QString highlighted;
        QString tooltip = model->data(model->index(index.row(), 0)).toString();
        char escaped[100];
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
        doc.setHtml(QString("<div>") % highlighted % "</div>");

        QStyleOptionViewItem newOption = option;
        newOption.state = option.state & (~QStyle::State_HasFocus);

        painter->save();
        newOption.widget->style()->drawControl(QStyle::CE_ItemViewItem, &newOption, painter);
        painter->translate(option.rect.left(), option.rect.top());
        doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex &) const override {
        return QSize(0, 48);
    }
};

class MyFilter: public QSortFilterProxyModel {
public:
    MyFilter(): QSortFilterProxyModel(nullptr) {
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        auto model = sourceModel();
        auto str = model->data(model->index(source_row, 0, source_parent)).toString();
        bool result = filterRegExp().pattern().size() == 0 || g_search.contains(str);
//        msg("%s: (%d) %d\n", str.toUtf8().toStdString().c_str(), filterRegExp().pattern().size(), result);
        return result;
    }

    void setFilter(QString &keyword) {
        setFilterFixedString(keyword);
    }
};

struct Action {
    QString id, tooltip, shortcut;
    Action(qstring id_, qstring tooltip_, qstring shortcut_): id(QString(id_.c_str())), tooltip(QString(tooltip_.c_str())), shortcut(QString(shortcut_.c_str())) {}
};

class QSearch: public QLineEdit {
    MyFilter *filter_;
public:
    QSearch(QWidget *parent, MyFilter *filter): QLineEdit(parent), filter_(filter) {
        setStyleSheet(R"(
                         QLineEdit, QLineEdit:hover, QLineEdit:active {
                             border-radius: 7px 7px 0 0;
                             font-size: 27px;
                             width: 720px;
                             height: 63px;
                             color: #eee;
                             border: none;
                         }
                         )");

        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);
    }

    void onTextChanged() {
        auto keyword = text();
        memset(&highlightTable, 0, sizeof(highlightTable));
        g_search.clear();
        if(keyword.size()) g_search.unite(g_intern[keyword[0].toLower().unicode()]);
        for(auto &i: keyword) {
            ushort c = i.toLower().unicode();
            if(!highlightTable[c]) {
                if(g_intern.contains(c))
                    g_search.intersect(g_intern[c]);
                else
                    g_search.clear();
            }
            highlightTable[c] = true;
        }
        filter_->setFilter(keyword);
    }

    bool event(QEvent *event) {
        switch(event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            switch (ke->key()) {
            case Qt::Key_Down:
                return true;
            case Qt::Key_Up:
                return true;
            default:
                return QLineEdit::event(event);
            }
        }
        default:
            return QLineEdit::event(event);
        }
    }
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

    QListView commands_;
    MyFilter proxy;

    QStandardItemModel model;
public:
    Qfred(): mainWindow_(nullptr), searchbox_(this, &proxy), model(0, 2) {
        setWindowFlags( Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        setStyleSheet(R"(
                      font-family: Segoe UI;
                      background: rgb(38, 44, 56);
                      QFrame {
                        border-radius: 7px;
                      }
                      )");

        populateList();
        commands_.setItemDelegate(new QItem());
        commands_.setLineWidth(0);
        commands_.setStyleSheet("border: none;");

        layout_.addWidget(&searchbox_);
        layout_.addWidget(&commands_);
        layout_.setContentsMargins(0, 0, 0, 0);
        layout_.setSpacing(0);

        setLayout(&layout_);

        resize(WIDTH, HEIGHT);

        CenterWidgets(this);
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
        proxy.setDynamicSortFilter(true);
        proxy.setSourceModel(&model);

        // TODO: cache it by id
        auto out = getActions();

        model.setRowCount(out->size());

        int i = 0;
        for(auto &item: *out) {
            model.setData(model.index(i, 0), item->tooltip);
            model.setData(model.index(i, 1), item->shortcut);
            model.setData(model.index(i, 2), item->id);

            internString(item->tooltip);
            i += 1;
        }

        commands_.setModel(&proxy);
        connect(&proxy, &MyFilter::dataChanged, &commands_, &QListView::reset);
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
    QfredContainer() {
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
    }
};

static QfredContainer widget;

//--------------------------------------------------------------------------
bool idaapi run(size_t)
{
  // hook_to_notification_point(HT_UI, ui_callback, &widget);
  widget.show();
  return true;
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

char wanted_hotkey[] = "Ctrl-Shift-P";


//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  0,                    // plugin flags
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
