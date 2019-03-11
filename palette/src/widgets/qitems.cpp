#include "qitems.h"

QItems::QItems(QWidget* parent)
	: QListView(parent),
	model_(new MyFilter(this)),
	item_delegate_(new QItem(this)) {

	setModel(model_);
	setItemDelegate(item_delegate_);
	setLineWidth(0);

	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setUniformItemSizes(true);
}
