#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include <utils.h>

class QItem : public QStyledItemDelegate {
public:
    QBrush item_hover_background_;
    QString style_sheet_;
    int item_height_, item_margin_left_, item_margin_top_;

    class ItemStyleObserver : public JSONObserver {
    public:
        ItemStyleObserver(QWidget* parent) : JSONObserver(parent, "theme/styles.json") {}

        void onUpdated(QJsonObject& styles) {
            QItem* owner = static_cast<QItem*>(parent());
            owner->updateConfig(styles);
        }
    };

    class ItemStyleSheetObserver : public QFileSystemWatcher {
        ItemStyleSheetObserver(QWidget* parent) {
            addPath("theme/item.css");
            connect(this, &QFileSystemWatcher::fileChanged, this, &ItemStyleSheetObserver::updated);
        }

        void updated() {
            QItem* owner = static_cast<QItem*>(parent());
            owner->setCSS(loadFile("theme/item.css"));
        }
    };

    QItem(QWidget* parent) : QStyledItemDelegate(parent) {
        updateConfig(json("theme/styles.json"));
    }

    void setCSS(const QString& css) {
        style_sheet_ = css;
    }

    void updateConfig(const QJsonObject& styles) {
        item_height_ = styles["itemHeight"].toInt();
        item_hover_background_ = QBrush(styles["itemHoverBackground"].toString().toStdString().c_str());
        item_margin_left_ = styles["itemMarginLeft"].toInt();
        item_margin_top_ = styles["itemMarginTop"].toInt();
        setCSS(loadFile("theme/item.css"));
    }

    void paint(QPainter* painter,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem&,
        const QModelIndex&) const override {
        return QSize(0, item_height_);
    }
};

#endif // QITEM_H
