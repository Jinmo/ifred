#include <filter.h>

void PaletteFilter::setFilter(const QString &keyword) {
  search_service_->search(keyword);
}

void PaletteFilter::onDoneSearching(QString keyword, QVector<Action> items,
                                    int recent_count) {
  emit layoutAboutToBeChanged();
  shown_items_ = items;
  keyword_ = keyword;
  emit layoutChanged();
  emit filteringDone(recent_count);
}

QModelIndex PaletteFilter::index(int row, int column,
                                 const QModelIndex &parent) const {
  return createIndex(row, column);
}

QVariant PaletteFilter::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole)
    return QVariant::fromValue(shown_items_[index.row()]);
  else if (role == Qt::UserRole)
    return keyword_;
  return QVariant();
}

int PaletteFilter::rowCount(const QModelIndex &parent) const {
  return shown_items_.count();
}

PaletteFilter::PaletteFilter(QWidget *parent, const QString &palette_name,
                             SearchService *search_service)
    : QAbstractItemModel(parent),
      shown_items_(),
      search_service_(search_service) {
  connect(search_service_, &SearchService::doneSearching, this,
          &PaletteFilter::onDoneSearching, Qt::QueuedConnection);
}
