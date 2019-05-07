#include <widgets/qitems.h>

QItems::QItems(QWidget* parent, const QVector<Action>& items)
    : QListView(parent),
    model_(new PaletteFilter(this, std::move(items))),
    item_delegate_(new QItem(this)) {

    QAbstractItemView::setModel(model_);
    QAbstractItemView::setItemDelegate(item_delegate_);

    setAttribute(Qt::WA_DeleteOnClose);

    // Border width
    setLineWidth(0);

    // Optimization
    setUniformItemSizes(true);

    // Smooth scroll
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setCurrentIndex(model_->index(0, 0));

    connect(model_, &PaletteFilter::filteringDone, this, [=](int index) {
        setCurrentIndex(model_->index(index, 0));
        });
}
