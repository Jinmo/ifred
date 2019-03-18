#include <widgets/qpalettecontainer.h>

QPaletteContainer::QPaletteContainer()
        : inner_stacked_(new QStackedWidget(this)), shadow_observer_(new ShadowObserver(this)) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

    shadow_observer_->activate();
    setCentralWidget(inner_stacked_);
}
