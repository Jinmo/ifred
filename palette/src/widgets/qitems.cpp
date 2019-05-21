#include <widgets/qitems.h>

QItems::QItems(QWidget* parent, const QString &palette_name, const QVector<Action>& items)
    : QListView(parent),
    model_(new PaletteFilter(this, palette_name, std::move(items))),
    item_delegate_(new QItem(this)) {

    QAbstractItemView::setModel(model_);
    QAbstractItemView::setItemDelegate(item_delegate_);

    //setAttribute(Qt::WA_DeleteOnClose);

    // Border width
    setLineWidth(0);

    // Optimization
    setUniformItemSizes(true);

    // Smooth scroll
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(model_, &PaletteFilter::filteringDone, this, [=](int index) {
        setCurrentIndex(model_->index(0, 0));
        item_delegate_->setRecents(index);
        });
}
