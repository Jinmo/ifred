#ifndef ITEMS_H
#define ITEMS_H

#include <QtGui>
#include <QtWidgets>

#include <widgets/item.h>
#include <widgets/palette_filter.h>
#include <action.h>

class Items : public QListView {
    PaletteFilter* model_;
    ItemDelegate* item_delegate_;

public:
    PaletteFilter* model() { return model_; }

    explicit Items(QWidget* parent, const QString& palette_name, const QVector<Action>& items);
    using QAbstractItemView::keyPressEvent;

    ~Items() {
        delete model_;
    }
};

#endif // ITEMS_H
