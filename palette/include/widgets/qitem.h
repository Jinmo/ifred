#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include <utils.h>

class QItem : public QStyledItemDelegate {
public:
    QBrush item_hover_background_;
    QString style_sheet_;
    int item_height_{}, item_margin_left_{}, item_margin_top_{};

    class ItemStyleObserver : public JSONObserver {
    public:
        explicit ItemStyleObserver(QItem* parent) : JSONObserver(parent, "theme/styles.json") {}

        void onUpdated(const QJsonObject& styles) override {
            auto * owner = dynamic_cast<QItem*>(parent());
            owner->updateConfig(styles);
        }
    } *item_style_observer_;

    class ItemStyleSheetObserver : public QFileSystemWatcher {
    public:
        explicit ItemStyleSheetObserver(QItem* parent): QFileSystemWatcher(parent) {
            addPath("theme/item.css");
            connect(this, &QFileSystemWatcher::fileChanged, this, &ItemStyleSheetObserver::onUpdated);
        }

        void onUpdated() {
            auto * owner = dynamic_cast<QItem*>(parent());
            owner->setCSS(loadFile("theme/item.css"));
        }
    } *item_stylesheet_observer_;

    explicit QItem(QWidget* parent)
    : QStyledItemDelegate(parent), item_style_observer_(new ItemStyleObserver(this)), item_stylesheet_observer_(new ItemStyleSheetObserver(this)) {
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

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
        return {0, item_height_};
    }
};

#endif // QITEM_H
