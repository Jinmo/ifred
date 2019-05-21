#include <utility>

#include <widgets/qpalette_inner.h>
#include <widgets/qpalettecontainer.h>

QPaletteInner::QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items, const QString& closeKey)
    : QFrame(parent), name_(name), shortcut_(nullptr) {
    QVBoxLayout* layout;

    // Create widgets
    searchbox_ = new QLineEdit(this);

    items_ = new QItems(this, name_, items);
    items_->setAttribute(Qt::WA_MacShowFocusRect, 0);
    searchbox_->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Add widgets
    layout = new QVBoxLayout(this);
    layout->addWidget(searchbox_);
    layout->addWidget(items_);

    //setAttribute(Qt::WA_DeleteOnClose);

    // Set margin and spacing between widgets
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

    setStyleSheet(loadFile("theme/window.css"));

    connect(searchbox_, &QLineEdit::returnPressed, [=]() {
        if (items_->model()->rowCount())
        {
            Action action = (items_->currentIndex()).data().value<Action>();
            window()->hide();
            emit itemClicked(action);
            close();
        }
        return true;
        });

    connect(searchbox_, &QLineEdit::textChanged, [=] { style()->polish(searchbox_); });

    connect(searchbox_, &QLineEdit::textChanged, [=](const QString & text) {
        items_->model()->setFilter(text);
        });

    items_->model()->setFilter(QString());

    connect(items_, &QListView::clicked, this, [=](const QModelIndex & index) {
        auto action = index.data().value<Action>();
        window()->hide();
        emit itemClicked(action);
        close();
        });

    searchbox_->installEventFilter(this);
    items_->installEventFilter(this);

    connect(this, &QPaletteInner::itemClicked, [=](Action & action) {
        emit items_->model()->search_service()->reportAction(action.id);
        });

    if (!closeKey.isEmpty()) {
        shortcut_ = registerShortcut({ closeKey }, [=]() {close(); });
    }

    registerShortcut({ "Ctrl+J" }, [=]() {arrowPressed(-1); });
    registerShortcut({ "Ctrl+K" }, [=]() {arrowPressed(+1); });
}

void QPaletteInner::arrowPressed(int delta) {
    auto new_row = items_->currentIndex().row() + delta;
    auto row_count = items_->model()->rowCount();

    if (new_row < 0)
        new_row = 0;
    else if (new_row >= row_count)
        new_row = row_count - 1;

    items_->setCurrentIndex(items_->model()->index(new_row, 0));
}

void QPaletteInner::keyPressEvent(QKeyEvent * e) {
    if (e->key() != Qt::Key_Escape)
        QFrame::keyPressEvent(e);
    else {
        close();
    }
}

bool QPaletteInner::eventFilter(QObject * obj, QEvent * event) {
    switch (event->type()) {
    case QEvent::KeyPress: {
        auto* keyEvent = dynamic_cast<QKeyEvent*>(event);

        QKeySequence sequence(keyEvent->key() | keyEvent->modifiers());
        if(registered_keys_.contains(sequence)) {
            emit registered_keys_[sequence]->activated();
            return true;
        }

        if (shortcut_ && QKeySequence(keyEvent->key() | keyEvent->modifiers()) == shortcut_->key()) {
            close();
            return true;
        }

        switch (keyEvent->key()) {
        case Qt::Key_Down:
        case Qt::Key_Up: {
            /* We manually process Up/Down key to handle corner cases and for ease of edits */
            arrowPressed(keyEvent->key() == Qt::Key_Down ? 1 : -1);
            return true;
        }
        case Qt::Key_Escape:
            /* This key closes the window */
            close();
            return true;
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

void QPaletteInner::showEvent(QShowEvent * event) {
    searchbox_->setFocus();
}

void QPaletteInner::closeEvent(QCloseEvent * event) {
    emit closed();
}

void QPaletteInner::setPlaceholderText(const QString & placeholder) {
    searchbox_->setPlaceholderText(placeholder);
}
