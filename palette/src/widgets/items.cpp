#include <widgets/items.h>

Items::Items(QWidget* parent, const QString &palette_name, const QVector<Action>& items)
    : QListView(parent),
    model_(new PaletteFilter(this, palette_name, std::move(items))),
    item_delegate_(new ItemDelegate(this)) {

    // Optimization
    setUniformItemSizes(true);

    QAbstractItemView::setModel(model_);
    QAbstractItemView::setItemDelegate(item_delegate_);

    setLineWidth(0);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(model_, &PaletteFilter::filteringDone, this, [=](int index) {
        setCurrentIndex(model_->index(0, 0));
        item_delegate_->setRecents(index);
        });
}
