#ifndef QITEM_H
#define QITEM_H

#include <QtGui>
#include <QtWidgets>

#include "common_defs.h"

class QItem : public QStyledItemDelegate
{
  public:

    QItem();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        auto model = index.model();
        QTextDocument doc;
        char *css = CSSLOADER_SYNC("item.css");
        doc.setDefaultStyleSheet(css);
        delete[] css;
        QString tooltip = model->data(model->index(index.row(), 0)).toString();
        QString highlighted;

        highlighted.reserve(tooltip.size() * 6);
        // char escaped[100];
        bool hl_state = false;
        for (auto &i : tooltip)
        {
            // QT::qsnprintf(escaped, sizeof(escaped), "&#x%04hx;", i.unicode());
            if (highlightTable[i.toLower().unicode()])
            {
                if (!hl_state)
                {
                    highlighted = highlighted % "<span>" % QString(i);
                    hl_state = true;
                }
                else
                {
                    highlighted = highlighted % QString(i);
                }
            }
            else
            {
                if (!hl_state)
                    highlighted = highlighted % QString(i);
                else
                {
                    highlighted = highlighted % "</span>" % QString(i);
                    hl_state = false;
                }
            }
        }
        //        msg("%s %08x\n", highlighted.toStdString().c_str(), option.state);
        doc.setHtml(QString("<div>") % highlighted % "</div>");

        painter->save();

        QStyleOptionViewItem newOption = option;
        newOption.state = option.state & (~QStyle::State_HasFocus);

        //        newOption.widget->style()->drawControl(QStyle::CE_ItemViewItem, &newOption, painter);
        painter->translate(option.rect.left(), option.rect.top());

        if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected))
        {
            painter->fillRect(0, 0, option.rect.width(), option.rect.height(), QBrush("#f5f5f5"));
        }

        painter->translate(28, 8);

        doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &,
                   const QModelIndex &) const override
    {
        return QSize(0, 48);
    }
};

#endif // QITEM_H
