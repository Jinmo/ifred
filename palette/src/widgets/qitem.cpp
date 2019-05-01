#include <widgets/qitem.h>
#include <widgets/palette_filter.h>

QHash<QString, QRegularExpression> capturing_regexp_cache;

QRegularExpression capturingRegexp(const QString & keyword) {
    QStringList regexp_before_join;

    if (capturing_regexp_cache.contains(keyword)) {
        return capturing_regexp_cache[keyword];
    }

    regexp_before_join << ("^");

    for (auto& x : keyword)
        if (!x.isSpace())
            regexp_before_join << (QString("(.*?)(") + x + ")");

    regexp_before_join.push_back("(.*)$");

    QRegularExpression result(regexp_before_join.join(""), QRegularExpression::CaseInsensitiveOption);

    capturing_regexp_cache[keyword] = result;
    return result;
}

const QString highlight(const QString& needle, const QString& haystack) {
    static QString em_("<em>"), emEnd_("</em>");
    QStringList highlights;

    if (needle.size()) {
        auto regexp = capturingRegexp(needle);
        auto match = regexp.match(haystack).capturedTexts();

        int i = -1;
        for (auto&& word : match) {
            if (++i == 0) continue;
            if (i % 2 == 0) highlights << em_ << word << emEnd_;
            else highlights << word;
        }
    }
    else {
        highlights << haystack;
    }

    return QString(highlights.join(""));
}

void QItem::paint(QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QTextDocument doc;

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    Action action = index.data().value<Action>();
    QString keyword = index.data(Qt::UserRole).value<QString>();

    doc.setDefaultStyleSheet(style_sheet_);
    doc.setDocumentMargin(0);

    auto html = "<div>" + highlight(keyword, action.description) + " <span>" + action.id + "</span></div>";
    doc.setHtml(html);

    painter->save();

    const QWidget* widget = option.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();

    opt.text = "";
    opt.state &= ~QStyle::State_HasFocus;
    opt.state |= QStyle::State_Active;
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    painter->restore();

    painter->save();

    auto textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &option, widget);
    painter->translate(textRect.left(), textRect.top());

    doc.drawContents(painter, QRectF(0, 0, opt.rect.width(), opt.rect.height()));
    painter->restore();
}

QSize QItem::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    const QWidget* widget = options.widget;
    QStyle* style = widget ? widget->style() : QApplication::style();
    QSize res = style->sizeFromContents(QStyle::CT_ItemViewItem, &options, QSize(), widget);

    return res;
}
