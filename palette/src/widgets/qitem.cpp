#include <widgets/qitem.h>
#include <widgets/myfilter.h>

QHash<QString, QRegExp> capturing_regexp_cache;

QRegExp capturingRegexp(const QString & keyword) {
    QStringList regexp_before_join;

    if (capturing_regexp_cache.contains(keyword)) {
        return capturing_regexp_cache[keyword];
    }

    regexp_before_join << ("^");

    for (auto& x : keyword)
        if (!x.isSpace())
            regexp_before_join << (QString("(.*?)(") + x + ")");

    regexp_before_join.push_back("(.*?)$");

    QRegExp result(regexp_before_join.join(""), Qt::CaseInsensitive);

    capturing_regexp_cache[keyword] = result;
    return result;
}

const QString highlight(const QString& keyword, const QString& tooltip) {
    static QString em_("<em>"), emEnd_("</em>");
    QStringList highlights;

    highlights << ("<div>");

    if (keyword.size()) {
        auto regexp = capturingRegexp(keyword);
        regexp.indexIn(tooltip);
        auto match = regexp.capturedTexts();

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

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    Action action = index.data().value<Action>();
    QString keyword = index.data(Qt::UserRole).value<QString>();

    doc.setDefaultStyleSheet(style_sheet_);
    doc.setDocumentMargin(0);
    // doc.setTextWidth(opt.rect.width());

    auto html = highlight(keyword, action.description()) + " <span>" + action.id() + "</span></div>";
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
