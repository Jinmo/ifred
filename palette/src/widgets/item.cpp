#include <filter.h>
#include <widgets/item.h>

QString escape(QString str) {
  str = str.replace("<", "&lt;");
  return str;
}

QString highlight(const QString& needle, const QString& haystack) {
  static QString em("<em>"), emEnd("</em>");
  QStringList highlights;

  if (needle.size()) {
    int pos = -1, last_pos = 0;
    for (auto c : needle) {
      pos = haystack.indexOf(c, pos + 1, Qt::CaseInsensitive);
      if (pos == -1) break;

      highlights << escape(haystack.mid(last_pos, pos - last_pos));
      highlights << em << escape(haystack[pos]) << emEnd;
      last_pos = pos + 1;
    }

    // push remaining
    highlights << haystack.mid(last_pos);
  } else {
    highlights << escape(haystack);
  }

  return QString(highlights.join(""));
}

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const {
  QStyledItemDelegate::paint(painter, option, index);

  static QHash<int, QString> classNameMap = {
      {QStyle::State_Selected, "selected"},
      {QStyle::State_MouseOver, "hover"},
      {QStyle::State_Selected | QStyle::State_MouseOver, "selected hover"}};

  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  auto action = index.data().value<Action>();
  auto keyword = index.data(Qt::UserRole).value<QString>();

  painter->save();

  auto* widget = option.widget;
  auto* style = widget->style();

  if (index.row() == recents_ - 1) {
    opt.state |= QStyle::State_On;
  }

  opt.text = "";
  opt.state &= ~QStyle::State_HasFocus;
  opt.state |= QStyle::State_Active;
  style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

  painter->restore();
  painter->save();

  auto textRect =
      style->subElementRect(QStyle::SE_ItemViewItemText, &option, widget);
  painter->translate(textRect.left(), textRect.top());

  // TODO: write a correct algorithm to limit rendering in QFrame, not
  // QMainWindow currently, I added this condition to prevent determining height
  // of topmost item
  if (textRect.top() >= 0)
    textRect = textRect.intersected(widget->contentsRect());

  auto document = const_cast<ItemDelegate*>(this)->renderAction(
      false,
      classNameMap[(int)opt.state &
                   (QStyle::State_Selected | QStyle::State_MouseOver)],
      keyword, action);

  document->drawContents(painter,
                         QRectF(0, 0, textRect.width(), textRect.height()));
  painter->restore();
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                             const QModelIndex& index) const {
  auto action = index.data().value<Action>();

  auto document = const_cast<ItemDelegate*>(this)->renderAction(
      true, QString(), QString(), action);
  document->setTextWidth(option.rect.width());

  return QSize(option.rect.width(), (int)document->size().height());
}

void ItemDelegate::updateCSS(const QString& style_sheet) {
  document_->setDefaultStyleSheet(style_sheet);
  QTextOption textOption;
  textOption.setWrapMode(QTextOption::WrapAnywhere);
  document_->setDefaultTextOption(textOption);
  document_->setDocumentMargin(0);
}

QTextDocument* ItemDelegate::renderAction(bool size_hint,
                                          const QString& className,
                                          const QString& keyword,
                                          Action& action) {
  QString html = "<table width=100% cellpadding=0 cellspacing=0 class=\"" +
                 className + "\"><tr><td class=\"name\">" +
                 (!size_hint ? highlight(keyword, action.name) : "keyword") +
                 "</td>";

  if (action.shortcut.size())
    html += "<td width=50px class=\"shortcut\" align=\"right\">" +
            action.shortcut + "</td>";

  html += "</tr>";

  if (action.description.size())
    html += "<tr><td class=\"description\" colspan=2>" + action.description +
            "</td></tr>";

  html += "</table>";
  document_->setHtml(html);
  return document_;
}
