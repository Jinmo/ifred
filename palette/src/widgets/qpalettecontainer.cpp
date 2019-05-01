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
    connect(this, &QPaletteContainer::show, this, &QPaletteContainer::onShow, Qt::QueuedConnection);
}

void QPaletteContainer::onShow(const QString &name, const QString &placeholder, const QVector<Action> &actions, ActionHandler func) {
    while (inner_stacked_->count())
        inner_stacked_->removeWidget(inner_stacked_->widget(0));

    auto delegate = new QPaletteInner(this, name, actions);
    connect(delegate, &QPaletteInner::enter_callback, std::move(func));
    inner_stacked_->addWidget(delegate);
    centerWidgets(this);

    delegate->searchbox().setPlaceholderText(placeholder);

    QMainWindow::show();
    activateWindow();
}
