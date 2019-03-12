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

class MyFilter : public QAbstractItemModel {
	QRegularExpression expression_;
	QString keyword_;

	QVector<Action> items_;
	QVector<int> shown_items_;
	int shown_items_count_;

public:
	MyFilter(QWidget* parent, const QVector<Action> &items)
		: QAbstractItemModel(parent), items_(std::move(items)), shown_items_count_(0), shown_items_(items.size()) {
		setFilter(QString());
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
		
		emit layoutAboutToBeChanged();

		for (auto& x : keyword)
			if (!x.isSpace())
				regexp_before_join.push_back(x);

		expression_ = QRegularExpression(regexp_before_join.join(".*"),
			QRegularExpression::CaseInsensitiveOption);

		// TODO: do chunk-wise item insertion, and sort using fuzzy match
		for (long i = 0; i < items_.size(); i++) {
			if (filterAcceptsRow(i)) {
				shown_items_[count++] = i;
			}
		}

		shown_items_count_ = count;
		emit layoutChanged();
	}

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
		return createIndex(row, column);
	};

	// we don't use this
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
		return QVariant::fromValue(items_[shown_items_[index.row()]]);
	}

	int	columnCount(const QModelIndex& parent = QModelIndex()) const override {
		return 1;
	}

	QModelIndex	parent(const QModelIndex& index) const override {
		return QModelIndex();
	}

	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const override {
		return shown_items_count_;
	}
};

#endif // MYFILTER_H
