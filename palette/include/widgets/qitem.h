#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include <observers.h>

class QItem : public QStyledItemDelegate {
    QString style_sheet_;
    QSize cached_size_;

    class ItemStyleSheetObserver : public QFileSystemWatcher {
        const char *css_;
    public:
        explicit ItemStyleSheetObserver(QItem* parent, const char *css): css_(css), QFileSystemWatcher(parent) {
            addPath(css);
            connect(this, &QFileSystemWatcher::fileChanged, this, &ItemStyleSheetObserver::onUpdated);
            onUpdated();
        }

        void onUpdated() {
            auto * owner = dynamic_cast<QItem*>(parent());
            owner->updateCSS(loadFile(css_));
        }
    } *item_stylesheet_observer_;

public:
    explicit QItem(QWidget* parent)
    : QStyledItemDelegate(parent), item_stylesheet_observer_(new ItemStyleSheetObserver(this, "theme/window.css")) {
    }

    void updateCSS(const QString &style_sheet) {
        style_sheet_ = style_sheet;
    }

    void paint(QPainter* painter,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // QITEM_H
