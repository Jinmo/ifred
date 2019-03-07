#include "qpalette_inner.h"
#include "common_defs.h"

QPaletteInner::QPaletteInner(QWidget *parent, QObject *)
        : QFrame(parent),
          css_observer_(new CSSObserver(this, "window.css")),
          layout_(new QVBoxLayout(this)),
          entries_(new QItems(this)),
          searchbox_(new QSearch(this, entries_->model())) {
    // TODO: we need to repaint the list item... I don't know how.
    // connect(entries_->model(), &MyFilter::dataChanged, entries_, [=]() {
    //     entries_->viewport()->update();
    // });

    layout_->addWidget(searchbox_);
    layout_->addWidget(entries_);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    entries_->setFixedHeight(HEIGHT);

    setLayout(layout_);
    resize(WIDTH, 0);

    connect(searchbox_, &QSearch::returnPressed, this, &QPaletteInner::onEnterPressed);
    connect(searchbox_, &QSearch::textChanged, this, &QPaletteInner::onTextChanged);

    searchbox_->installEventFilter(this);
}

void QPaletteInner::processEnterResult(EnterResult res) {
    if (res.hide()) {
        if (res.nextPalette()) {
            // TODO
        } else {
            Q_ASSERT(window() != nullptr);
            window()->close();
        }
    } else {
        // hide=true if nextPalette != NULL
        Q_ASSERT(!res.nextPalette());
    }
}
