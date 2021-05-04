#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include <palette/action.h>
#include <palette/observers.h>

class ItemDelegate : public QStyledItemDelegate {
    QTextDocument* document_;
    int recents_;

    bool style_updated_;
    QSize cached_size_;

public:
    explicit ItemDelegate(QWidget* parent)
        : QStyledItemDelegate(parent), document_(new QTextDocument(this)), recents_(0), style_updated_(false) {
        updateCSS(loadFile("theme/window.css"));
    }

    void updateCSS(const QString& style_sheet);

    void paint(QPainter* painter,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QTextDocument* renderAction(bool just_return, const QString& className, const QString& keyword, Action& action);

    void setRecents(int index) { recents_ = index; }
};

#endif // QITEM_H
