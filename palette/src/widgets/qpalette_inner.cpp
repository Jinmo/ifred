#include <widgets/qpalette_inner.h>
#include <widgets/qpalettecontainer.h>

QPaletteInner::QPaletteInner(QWidget* parent, const QVector<Action>& items)
    : QFrame(parent),
    css_observer_(new CSSObserver(this, "theme/window.css")),
    styles_observer_(new StylesObserver(this)),
    layout_(new QVBoxLayout(this)),
    entries_(new QItems(this, items)),
    searchbox_(new QSearch(this, entries_)) {

    // Add widgets
    layout_->addWidget(searchbox_);
    layout_->addWidget(entries_);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    setLayout(layout_);

    connect(searchbox_, &QSearch::returnPressed, this, &QPaletteInner::onEnterPressed);
    connect(searchbox_, &QSearch::textChanged, this, &QPaletteInner::onTextChanged);

    connect(entries_, &QListView::clicked, this, &QPaletteInner::itemClicked);

    searchbox_->installEventFilter(this);
    entries_->installEventFilter(this);
}

void QPaletteInner::processEnterResult(EnterResult res) {
    if (res.hide()) {
        if (res.nextPalette()) {
            container()->set_inner(res.nextPalette());
        }
        else {
            container()->close();
        }
    }
    else {
        // hide=true if nextPalette != NULL
        Q_ASSERT(!res.nextPalette());
    }
}

void QPaletteInner::closeWindow() {
    container()->close();
}

QPaletteContainer *QPaletteInner::container() {
    return dynamic_cast<QPaletteContainer *>(window());
}
