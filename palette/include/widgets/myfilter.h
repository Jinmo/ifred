#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

class MyFilter : public QAbstractItemModel {
	QRegularExpression expression_;
	QString keyword_;

public:
	QVector<Action> items_;
	QVector<int> shown_items_;
	int shown_items_count_;

	QFutureWatcher<bool> *watcher_;
	QFuture<bool> future_;

	MyFilter(QWidget* parent, const QVector<Action>& items)
		: QAbstractItemModel(parent), items_(std::move(items)), shown_items_count_(0), shown_items_(items.size()), future_(), watcher_(new QFutureWatcher<bool>()) {
		setFilter(QString());

		connect(watcher_, &QFutureWatcher<bool>::finished, this, &MyFilter::filteringDone);
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

	QModelIndex index(int row) const {
		return createIndex(row, 0, nullptr);
	}

	void setFilter(QString& keyword) {
		static QRegExp emptyRegExp;
		keyword_ = keyword;

		QStringList regexp_before_join;

		emit layoutAboutToBeChanged();

		for (auto& x : keyword)
			if (!x.isSpace())
				regexp_before_join.push_back(x);

		expression_ = QRegularExpression(regexp_before_join.join(".*"),
			QRegularExpression::CaseInsensitiveOption);

		startFiltering();
	}

	void startFiltering() {
		watcher_->cancel();
		watcher_->setFuture(QtConcurrent::run(this, &MyFilter::update_filter, keyword_));
	}

	void filteringDone() {
		emit layoutChanged();
	}

	bool update_filter(QString& keyword) {
		long count = 0;

		// TODO: do chunk-wise item insertion, and sort using fuzzy match
		for (long i = 0; i < items_.size(); i++) {
			if (filterAcceptsRow(i)) {
				shown_items_[count++] = i;
			}
		}

		shown_items_count_ = count;
		return true;
	}

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override {
		return createIndex(row, column);
	};

	// we don't use this
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override {
		return QVariant::fromValue(items_[shown_items_[index.row()]]);
	}

	int	columnCount(const QModelIndex & parent = QModelIndex()) const override {
		return 1;
	}

	QModelIndex	parent(const QModelIndex & index) const override {
		return QModelIndex();
	}

	virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const override {
		return shown_items_count_;
	}
};

#endif // MYFILTER_H
