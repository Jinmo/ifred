#include <widgets/qitems.h>

QItems::QItems(QWidget* parent, const QVector<Action> &items)
	: QListView(parent),
	model_(new MyFilter(this, std::move(items))),
	item_delegate_(new QItem(this)) {

	setModel(model_);
	setItemDelegate(item_delegate_);
	setLineWidth(0);

	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setUniformItemSizes(true);
}
