#ifndef QITEMS_H
#define QITEMS_H

#include <QtGui>
#include <QtWidgets>
#include "common_defs.h"
#include "myfilter.h"
#include "qitem.h"

class QItems : public QListView, public FredCallback
{
    MyFilter filter_;
    QStandardItemModel source_;
    QItem item_delegate_;

public:
    auto &model() { return filter_; }
    auto &source() { return source_; }
    QItems();
};

#endif // QITEMS_H
