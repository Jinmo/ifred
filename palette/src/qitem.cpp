#include "qitem.h"

#include "common_defs.h"

QCache<QPair<QString, QString>, QString> hlCache;

QString &highlight(QString &keyword, QString &tooltip) {
    static QString em_("<em>"), emEnd_("</em>");
    auto cache_key = QPair<QString, QString>(keyword, tooltip);

    if (hlCache.contains(cache_key))
        return *hlCache[cache_key];

    QStringList highlights;
    int i, j = 0, start = 0;
    bool toggle = false;

    highlights.push_back("<div>");

    if (keyword.size()) {
        for (i = 0; i < tooltip.size(); i++) {
            auto c = tooltip[i];
            if (c.toLower() == keyword[j].toLower()) {
                // start of highlighted text
                if (!toggle) {
                    highlights << (tooltip.mid(start, i - start));
                    start = i;
                }
                ++j;
                if (j == keyword.size()) {
                    highlights << (em_);
                    highlights << tooltip.mid(start, i++ + 1 - start);
                    highlights << (emEnd_);
                    toggle = false;
                    break;
                }
                toggle = true;
            } else {
                if (toggle) {
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

    QString *result = new QString(highlights.join(""));
    hlCache.insert(cache_key, result);
    return *result;
}

void QItem::paint(QPainter *painter,
                  const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto model = index.model();
    QTextDocument doc;
    auto &css = loadFile("item.css");
    doc.setDefaultStyleSheet(css);

    QString tooltip = model->data(model->index(index.row(), 0)).toString();
    QString id = model->data(model->index(index.row(), 2)).toString();
    QString keyword = g_keyword;

    auto html = highlight(keyword, tooltip) + "<span>" + id + "</span>";

    doc.setHtml(html);
    painter->save();

    QStyleOptionViewItem newOption = option;
    newOption.state = option.state & (~QStyle::State_HasFocus);

    painter->translate(option.rect.left(), option.rect.top());

    auto json_ = config();

    if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
        painter->fillRect(0, 0, option.rect.width(), option.rect.height(),
                          QBrush(json_["itemHoverBackground"].toString().toStdString().c_str()));
    }

    painter->translate(json_["itemMarginLeft"].toInt(), json_["itemMarginTop"].toInt());

    doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
    painter->restore();
}
