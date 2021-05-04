#include <utility>
#include <widgets/palette.h>
#include <search_services/basic_service.h>

PaletteFrame::PaletteFrame(QWidget* parent, const QString& name, const QString& closeKey, SearchService *search_service)
    : QFrame(parent), name_(name), shortcut_(nullptr) {
    QVBoxLayout* layout;

    // Create widgets
    searchbox_ = new QLineEdit(this);
    searchbox_->setAttribute(Qt::WA_MacShowFocusRect, 0);

    items_ = new PaletteItems(this, name_, search_service);
    items_->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Add widgets
    layout = new QVBoxLayout(this);
    layout->addWidget(searchbox_);
    layout->addWidget(items_);

    // Set margin and spacing between widgets
    // This is just like normalize.css because it can be overriden by css
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

    connect(searchbox_, &QLineEdit::returnPressed, [=]() {
        if (items_->model()->rowCount())
        {
            Action action = (items_->currentIndex()).data().value<Action>();
            window()->hide();
            emit itemClicked(action);
            window()->close();
        }
        return true;
        });

    connect(searchbox_, &QLineEdit::textChanged, [=](const QString & text) {
        style()->polish(searchbox_);
        items_->model()->setFilter(text);
        });

    connect(items_, &QListView::clicked, this, [=](const QModelIndex & index) {
        auto action = index.data().value<Action>();
        window()->hide();
        emit itemClicked(action);
        window()->close();
        });

    searchbox_->installEventFilter(this);
    items_->installEventFilter(this);

    connect(this, &PaletteFrame::itemClicked, [=](Action & action) {
        emit items_->model()->search_service()->itemClicked(action.id);
        });

    items_->model()->setFilter(QString());

    if (!closeKey.isEmpty()) {
        shortcut_ = registerShortcut({ closeKey }, [=]() {window()->close(); });
    }

    registerShortcut({ "Ctrl+J" }, [=]() {arrowPressed(-1); });
    registerShortcut({ "Ctrl+K" }, [=]() {arrowPressed(+1); });

    /* This key closes the window */
    registerShortcut({ "Esc" }, [=]() {window()->close(); });
}

void PaletteFrame::arrowPressed(int delta) {
    auto new_row = items_->currentIndex().row() + delta;
    auto row_count = items_->model()->rowCount();

    if (new_row < 0)
        new_row = 0;
    else if (new_row >= row_count)
        new_row = row_count - 1;

    items_->setCurrentIndex(items_->model()->index(new_row, 0));
}

bool PaletteFrame::eventFilter(QObject * obj, QEvent * event) {
    switch (event->type()) {
    case QEvent::KeyPress: {
        auto* keyEvent = dynamic_cast<QKeyEvent*>(event);

        QKeySequence sequence(keyEvent->key() | keyEvent->modifiers());
        if (registered_keys_.contains(sequence)) {
            emit registered_keys_[sequence]->activated();
            return true;
        }

        switch (keyEvent->key()) {
        case Qt::Key_Down:
        case Qt::Key_Up: {
            /* We manually process Up/Down key to handle corner cases and for ease of edits */
            arrowPressed(keyEvent->key() == Qt::Key_Down ? 1 : -1);
            return true;
        }
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
            /* Forward QKeyEvent for PageDown/PageUp key since QAbstractListView already implemented the procedure */
            event->ignore();
            items_->keyPressEvent(keyEvent);
            return true;
        default:
            return obj->eventFilter(obj, event);
        }
    }
    case QEvent::FocusOut: {
        auto focusEvent = static_cast<QFocusEvent*>(event);
        if (obj == searchbox_ && focusEvent->reason() == Qt::MouseFocusReason) {
            searchbox_->setFocus();
            return true;
        }
        else {
            return false;
        }
    }
    case QEvent::ShortcutOverride: {
        auto* keyEvent = dynamic_cast<QKeyEvent*>(event);
        /*
        Handling ShortcutOverride prevents UI from hooking the shortcuts used in the command palette.
        If the shortcut is registered by registerShortcut(), it's not overriden.
        */
        event->accept();
        return true;
    }
    default:
        return QFrame::eventFilter(obj, event);
    }
}

void PaletteFrame::showEvent(QShowEvent * event) {
    searchbox_->setFocus();
}

void PaletteFrame::setPlaceholderText(const QString & placeholder) {
    searchbox_->setPlaceholderText(placeholder);
}

static void centerWidgets(QWidget * window, QWidget * widget, QWidget * host = nullptr) {
    if (!host)
        host = widget->parentWidget();

    if (host) {
        auto hostRect = host->geometry();
        window->move(hostRect.center() - widget->rect().center());
    }
    else {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        window->move(x, y);
    }
}

CommandPalette::CommandPalette(QWidget * parent)
    : QMainWindow(parent) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

    // Set style sheet from window.css
    setStyleSheet(loadFile("theme/window.css"));

    // Set shadow effect from styles.json
    auto * shadow = new QGraphicsDropShadowEffect(this);

    int shadowWidth = json("theme/styles.json")["shadow-width"].toInt();
    QColor shadowColor(0, 0, 0, 100);

    shadow->setBlurRadius(shadowWidth);
    shadow->setColor(shadowColor);
    shadow->setOffset(0);

    setGraphicsEffect(shadow);
    setContentsMargins(shadowWidth, shadowWidth, shadowWidth, shadowWidth);
}

void CommandPalette::show(const QString & name, const QString & placeholder, const QVector<Action> & actions, const QString & closeKey, ActionHandler func) {
    auto inner = new PaletteFrame(this, name, closeKey, new BasicService(nullptr, name, actions));

    setCentralWidget(inner);
    connect(inner, &PaletteFrame::itemClicked, std::move(func));

    inner->setPlaceholderText(placeholder);

    QMainWindow::show();
    centerWidgets(this, this, nullptr);

    activateWindow();
}
