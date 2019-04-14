#include <widgets/qitem.h>
#include <widgets/myfilter.h>

QHash<QString, QRegularExpression> capturing_regexp_cache;

QRegularExpression genCapturingRegexp(const QString & keyword) {
    QStringList regexp_before_join;

    if (capturing_regexp_cache.contains(keyword)) {
        return capturing_regexp_cache[keyword];
    }

    regexp_before_join << ("^");

    for (auto& x : keyword)
        if (!x.isSpace())
            regexp_before_join << (QString("(.*?)(") + x + ")");

    regexp_before_join.push_back("(.*?)$");

    QRegularExpression result(regexp_before_join.join(""),
                              QRegularExpression::CaseInsensitiveOption);

    capturing_regexp_cache[keyword] = result;
    return result;
}

const QString highlight(const QString& keyword, const QString& tooltip) {
    static QString em_("<em>"), emEnd_("</em>");
    auto cache_key = QPair<QString, QString>(keyword, tooltip);

    QStringList highlights;

    highlights << ("<div>");

    if (keyword.size()) {
        auto regexp = genCapturingRegexp(keyword);
        auto match = regexp.match(tooltip).capturedTexts();

        int i = -1;
        for (auto&& word : match) {
            if (++i == 0) continue;
            if (i % 2 == 0) highlights << em_ << word << emEnd_;
            else highlights << word;
        }
    }
    else {
        highlights << tooltip;
    }

    return QString(highlights.join(""));
}

void QItem::paint(QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QTextDocument doc;

    Action action = index.data().value<Action>();
    QString keyword = index.data(Qt::UserRole).value<QString>();

    doc.setDefaultStyleSheet(style_sheet_);
    doc.setDocumentMargin(0);

    auto html = highlight(keyword, action.description()) + "<span>" + action.id() + "</span></div>";
    doc.setHtml(html);

    painter->save();
    painter->translate(option.rect.left(), option.rect.top());

    if (option.state & (QStyle::State_HasFocus | QStyle::State_Selected)) {
        painter->fillRect(0, 0, option.rect.width(), option.rect.height(),
            item_hover_background_);
    }

    painter->translate(item_margin_left_, item_margin_top_);

    doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));
    painter->restore();
}
