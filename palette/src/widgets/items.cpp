#include <widgets/palette_items.h>

PaletteItems::PaletteItems(QWidget* parent, const QString& palette_name,
                           SearchService* search_service)
    : QListView(parent),
      model_(new PaletteFilter(this, palette_name, search_service)),
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
