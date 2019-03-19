#include <widgets/qpalettecontainer.h>

static void centerWidgets(QWidget* widget, QWidget* host = nullptr) {
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

QPaletteContainer::QPaletteContainer()
        : inner_stacked_(new QStackedWidget(this)), shadow_observer_(new ShadowObserver(this)) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

    setCentralWidget(inner_stacked_);
}

void QPaletteContainer::set_inner(QPaletteInner *delegate) {
    while (inner_stacked_->count())
        inner_stacked_->removeWidget(inner_stacked_->widget(0));
    inner_stacked_->addWidget(delegate);
}

void QPaletteContainer::show(bool focus) {
    centerWidgets(this);
    QMainWindow::show();

    if(focus)
        activate();
}

void QPaletteContainer::activate() {
    activateWindow();

    if (!inner_stacked_->count())
        return;

    auto * inner = static_cast<QPaletteInner*>(inner_stacked_->widget(0));

    inner->searchbox().selectAll();
    inner->searchbox().setFocus();
}
