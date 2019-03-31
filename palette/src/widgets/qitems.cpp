#include <widgets/qitems.h>

QItems::QItems(QWidget* parent, const QVector<Action>& items)
    : QListView(parent),
    model_(new MyFilter(this, std::move(items))),
    item_delegate_(new QItem(this)) {

    QAbstractItemView::setModel(model_);
    setItemDelegate(item_delegate_);
    setLineWidth(0);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setUniformItemSizes(true);

    connect(model_, &MyFilter::filteringDone, this, [=](int index) {
        setCurrentIndex(model_->index(index, 0));
        });
}
