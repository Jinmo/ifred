#include "qitem.h"
#include "myfilter.h"

QCache<QPair<QString, QString>, QString> hlCache;
extern QString g_keyword;

QString& highlight(QString& keyword, QString& tooltip) {
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
			}
			else {
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
	}
	else {
		highlights << tooltip;
	}
	highlights << ("</div>");

	QString* result = new QString(highlights.join(""));
	hlCache.insert(cache_key, result);
	return *result;
}

void QItem::paint(QPainter * painter,
	const QStyleOptionViewItem & option, const QModelIndex & index) const {
	QTextDocument doc;

	auto model = index.model();
	Action action = model->data(model->index(index.row(), 0)).value<Action>();

	doc.setDefaultStyleSheet(style_sheet_);

	auto html = highlight(g_keyword, action.description()) + "<span>" + action.id() + "</span>";
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
