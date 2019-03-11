#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>

class Action {
public:
	Action() {}
	Action(const Action& other) : id_(other.id_), description_(other.description_), shortcut_(other.shortcut_) {
	}
	~Action() {}

	Action(const QString& id, const QString& description, const QString& shortcut)
		: id_(id),
		description_(description),
		shortcut_(shortcut) {}

	QString& id() { return id_; }
	QString& description() { return description_; }
	QString& shortcut() { return shortcut_; }

private:
	QString id_, description_, shortcut_;
};

Q_DECLARE_METATYPE(Action);

class MyFilter : public QStandardItemModel {
	QRegularExpression expression_;
	QString keyword_;

	QVector<Action> items_;

public:
	MyFilter(QWidget* parent) : QStandardItemModel(parent) {
	}

	bool filterAcceptsRow(int source_row) {
		if (keyword_.isEmpty())
			return true;

		const QString& str = items_[source_row].description();
		bool result = str.contains(expression_);
		return result;
	}

	bool lessThan(const QModelIndex& left,
		const QModelIndex& right) const;

	void setFilter(QString& keyword) {
		static QRegExp emptyRegExp;
		long count = 0;
		keyword_ = keyword;

		QStringList regexp_before_join;

		for (auto& x : keyword)
			if (!x.isSpace())
				regexp_before_join.push_back(x);

		expression_ = QRegularExpression(regexp_before_join.join(".*"),
			QRegularExpression::CaseInsensitiveOption);

		setRowCount(items_.size());
		for (long i = 0; i < items_.size(); i++) {
			if (filterAcceptsRow(i)) {
				setData(index(count++, 0), QVariant::fromValue(items_[i]));
			}
		}
		setRowCount(count);
	}

	void populate(const QVector<Action>& items) {
		items_ = items;
		qDebug() << items_.size();

		setRowCount(items_.size());
		setColumnCount(1);
		setFilter(keyword_);
	}
};

#endif // MYFILTER_H
