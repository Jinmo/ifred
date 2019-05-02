#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include <observers.h>

class QItem : public QStyledItemDelegate {
    QString style_sheet_;
    QSize cached_size_;

public:
    explicit QItem(QWidget* parent)
        : QStyledItemDelegate(parent) {
        updateCSS(loadFile("theme/window.css"));
    }

    void updateCSS(const QString& style_sheet) {
        style_sheet_ = style_sheet;
    }

    void paint(QPainter* painter,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // QITEM_H
