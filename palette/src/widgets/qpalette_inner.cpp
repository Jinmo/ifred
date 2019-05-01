#include <utility>

#include <widgets/qpalette_inner.h>
#include <widgets/qpalettecontainer.h>

QPaletteInner::QPaletteInner(QWidget* parent, const QString &name, const QVector<Action>& items)
    : QFrame(parent), name_(name) {
    QVBoxLayout* layout_;

    // Create widgets
    items_ = new QItems(this, items);
    layout_ = new QVBoxLayout(this);
    searchbox_ = new QLineEdit(this);

    // Add widgets
    layout_->addWidget(searchbox_);
    layout_->addWidget(items_);

    // Set margin and spacing between widgets
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    setLayout(layout_);
    setStyleSheet(loadFile("theme/window.css"));

    connect(searchbox_, &QLineEdit::returnPressed, this, &QPaletteInner::onEnterPressed);
    connect(searchbox_, &QLineEdit::textChanged, [=](const QString &text) {
        items_->scrollToTop();
        items_->model()->setFilter(text);
    });

    connect(items_, &QListView::clicked, this, [=](const QModelIndex & index) {
        auto action = index.data().value<Action>();
        close();
        emit itemClicked(action);
    });

    searchbox_->installEventFilter(this);
    items_->installEventFilter(this);
}

void QPaletteInner::keyPressEvent(QKeyEvent* e) {
    if (e->key() != Qt::Key_Escape)
        QFrame::keyPressEvent(e);
    else {
        close();
    }
}

bool QPaletteInner::eventFilter(QObject * obj, QEvent * event) {
    switch (event->type()) {
    case QEvent::KeyPress: {
        auto* ke = dynamic_cast<QKeyEvent*>(event);
        switch (ke->key()) {
        case Qt::Key_Down:
        case Qt::Key_Up: {
            /* We manually process Up/Down key to handle corner cases and for ease of edits */
            int delta = ke->key() == Qt::Key_Down ? 1 : -1;
            auto new_row = items_->currentIndex().row() + delta;

            if (new_row == -1)
                new_row = 0;
            else if (new_row == items_->model()->rowCount())
                new_row--;

            items_->setCurrentIndex(items_->model()->index(new_row, 0));
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
            items_->keyPressEvent(ke);
            return true;
        default:
            return obj->eventFilter(obj, event);
        }
    }
    case QEvent::ShortcutOverride: {
        /* Handling ShortcutOverride prevents UI from hooking the shortcuts used in the command palette */
        event->accept();
        return true;
    }
    default:
        return QFrame::eventFilter(obj, event);
    }
}

bool QPaletteInner::onEnterPressed() {
    if (items_->model()->rowCount())
    {
        Action action = (items_->currentIndex()).data().value<Action>();
        close();
        emit itemClicked(action);
    }
    return true;
}

void QPaletteInner::showEvent(QShowEvent *event) {
    searchbox_->setFocus();
}

void QPaletteInner::setPlaceholderText(const QString &placeholder) {
    searchbox_->setPlaceholderText(placeholder);
}
