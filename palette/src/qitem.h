#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include "utils.h"

class QItem : public QStyledItemDelegate {
public:
    QItem(QWidget *parent) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex &) const override {
        return QSize(0, config()["itemHeight"].toInt());
    }
};

#endif // QITEM_H
