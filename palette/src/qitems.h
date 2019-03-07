#ifndef QITEMS_H
#define QITEMS_H

#include <QtGui>
#include <QtWidgets>
#include "myfilter.h"
#include "qitem.h"

class QItems : public QListView {
    MyFilter *filter_;
    QStandardItemModel *source_;
    QItem *item_delegate_;

public:
    MyFilter *model() { return filter_; }

    QStandardItemModel *source() { return source_; }

    explicit QItems(QWidget *parent);
};

#endif // QITEMS_H
