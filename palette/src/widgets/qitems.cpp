#include <widgets/qitems.h>

QItems::QItems(QWidget* parent, const QVector<Action>& items)
    : QListView(parent),
    model_(new PaletteFilter(this, std::move(items))),
    item_delegate_(new QItem(this)) {

    QAbstractItemView::setModel(model_);

    setItemDelegate(item_delegate_);
    setLineWidth(0);

    // Optimization
    setUniformItemSizes(true);

    // Smooth scroll
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(model_, &PaletteFilter::filteringDone, this, [=](int index) {
        setCurrentIndex(model_->index(index, 0));
        });
}
