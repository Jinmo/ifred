#include <widgets/items.h>

Items::Items(QWidget* parent, const QString& palette_name, const QVector<Action>& items, SearchService *search_service)
    : QListView(parent),
    model_(new PaletteFilter(this, palette_name, std::move(items), search_service)),
    item_delegate_(new ItemDelegate(this)) {

    // Optimization
    setUniformItemSizes(true);

    setLineWidth(0);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QAbstractItemView::setModel(model_);
    QAbstractItemView::setItemDelegate(item_delegate_);

    connect(model_, &PaletteFilter::filteringDone, this, [=](int index) {
        item_delegate_->setRecents(index);
        setCurrentIndex(model_->index(0, 0));
        });
}
