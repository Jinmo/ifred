#include "qitem.h"
#include "utils.h"

#include "common_defs.h"

void QItem::paint(QPainter *painter,
                  const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto model = index.model();
    QTextDocument doc;
    auto &css = loadFile("item.css");
    doc.setDefaultStyleSheet(css);
    QString tooltip = model->data(model->index(index.row(), 0)).toString();

    QString em("<em>"), emEnd("</em>");
    QString em_(em), emEnd_(emEnd);

    QStringList highlights;
    int i, j = 0, start = 0;
    bool toggle = false;

    highlights.push_back("<div>");

    if (g_keyword.size())
    {
        for (i = 0; i < tooltip.size(); i++)
        {
            auto c = tooltip[i];
            if (c.toLower() == g_keyword[j].toLower())
            {
                // start of highlighted text
                if (!toggle)
                {
                    highlights << (tooltip.mid(start, i - start));
                    start = i;
                }
                ++j;
                if (j == g_keyword.size())
                {
                    highlights << (em_);
                    highlights << tooltip.mid(start, i++ + 1 - start);
                    highlights << (emEnd_);
                    toggle = false;
                    break;
                }
                toggle = true;
            }
            else
            {
                if (toggle)
                {
                    highlights << (em_);
                    highlights << tooltip.mid(start, i - start);
                    highlights << (emEnd_);
                    start = i;
                }
                toggle = false;
            }
        }
        if (toggle)
            highlights << (em_);
        highlights << tooltip.mid(i, tooltip.size() - i);
        if (toggle)
            highlights << (emEnd_);
    } else {
        highlights << tooltip;
    }
    highlights << ("</div>");

    doc.setHtml(highlights.join(""));
    painter->save();

    QStyleOptionViewItem newOption = option;
    newOption.state = option.state & (~QStyle::State_HasFocus);

    painter->translate(option.rect.left(), option.rect.top());

    auto json_ = config();

    if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected))
    {
        painter->fillRect(0, 0, option.rect.width(), option.rect.height(),
                          QBrush(json_["itemHoverBackground"].toString().toStdString().c_str()));
    }

    painter->translate(json_["itemMarginLeft"].toInt(), json_["itemMarginTop"].toInt());

    doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
    painter->restore();
}
