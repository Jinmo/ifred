#include "qitems.h"

QItems::QItems(QWidget *parent)
: QListView(parent),
filter_(new MyFilter(this)),
source_(new QStandardItemModel(this)),
item_delegate_(new QItem(this))
{
    filter_->setDynamicSortFilter(false);
    filter_->setSourceModel(source_);

    setModel(filter_);
    setItemDelegate(item_delegate_);
    setLineWidth(0);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}
