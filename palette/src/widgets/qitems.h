#ifndef QITEMS_H
#define QITEMS_H

#include <QtGui>
#include <QtWidgets>
#include <widgets/qitem.h>
#include <widgets/myfilter.h>

class MyFilter;

class QItems : public QListView {
	MyFilter* model_;
	QItem* item_delegate_;

public:
	MyFilter* model() { return model_; }

	explicit QItems(QWidget* parent);
};

#endif // QITEMS_H
