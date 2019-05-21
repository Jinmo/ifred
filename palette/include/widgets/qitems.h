#ifndef QITEMS_H
#define QITEMS_H

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitem.h>
#include <widgets/palette_filter.h>
#include <action.h>

class QItems : public QListView {
    PaletteFilter* model_;
    QItem* item_delegate_;

public:
    PaletteFilter* model() { return model_; }

    explicit QItems(QWidget* parent, const QString &palette_name, const QVector<Action>& items);
    using QAbstractItemView::keyPressEvent;

    ~QItems() {
        delete model_;
    }
};

#endif // QITEMS_H
