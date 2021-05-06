#ifndef ITEMS_H
#define ITEMS_H

#include <palette/action.h>
#include <palette/filter.h>
#include <palette/widgets/item.h>

#include <QtGui>
#include <QtWidgets>

class PaletteItems : public QListView {
  PaletteFilter* model_;
  ItemDelegate* item_delegate_;

 public:
  PaletteFilter* model() { return model_; }

  explicit PaletteItems(QWidget* parent, const QString& palette_name,
                        SearchService* search_service);
  using QAbstractItemView::keyPressEvent;

  ~PaletteItems() { delete model_; }
};

#endif  // ITEMS_H
