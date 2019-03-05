#include "qitem.h"

QItem::QItem()
{
}

void QItem::paint(QPainter *painter,
           const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto model = index.model();
    QTextDocument doc;
    auto &css = loadFile("item.css");
    doc.setDefaultStyleSheet(css);
    QString tooltip = model->data(model->index(index.row(), 0)).toString();
    QString highlighted;

    char localTable[65536];

    memcpy(localTable, highlightTable, sizeof(localTable));

    highlighted.reserve(tooltip.size() * 6);
    QStringList hl_list;
    bool hl_state = false;
    auto *table = new short[tooltip.size()];
    
    for (auto &i : tooltip)
    {
        auto c = i.toLower().unicode();
        // QT::qsnprintf(escaped, sizeof(escaped), "&#x%04hx;", i.unicode());
        if (localTable[c])
        {
            localTable[c]--;
            if (!hl_state)
            {
                highlighted = highlighted % "<em>" % QString(i);
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
                highlighted = highlighted % "</em>" % QString(i);
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
