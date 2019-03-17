#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

int distance(const QString& s1_, const QString& s2_);

class MyFilter : public QAbstractItemModel {
public:
    const QVector<Action> items_;
    QVector<int> shown_items_;
    QVector<int> shown_items_temp_;
    int shown_items_count_;

    QFutureWatcher<bool>* watcher_;
    QFuture<bool> future_;

    MyFilter(QWidget* parent, const QVector<Action>& items)
        : QAbstractItemModel(parent), items_(std::move(items)), shown_items_count_(0),
        shown_items_(items.size()), shown_items_temp_(items.size()), future_(), watcher_(new QFutureWatcher<bool>()) {
        setFilter(QString());

        connect(watcher_, &QFutureWatcher<bool>::finished, this, &MyFilter::filteringDone);
    }

    bool filterAcceptsRow(const QString& keyword, QRegularExpression regexp, int source_row) {
        if (keyword.isEmpty())
            return true;

        const QString& str = items_[source_row].description();
        bool result = str.contains(regexp);
        return result;
    }

    bool lessThan(const QModelIndex& left,
        const QModelIndex& right) const;

    QModelIndex index(int row) const {
        return createIndex(row, 0, nullptr);
    }

    QRegularExpression genRegexp(const QString& keyword) {
        QStringList regexp_before_join;

        for (auto& x : keyword)
            if (!x.isSpace())
                regexp_before_join.push_back(x);

        return QRegularExpression(regexp_before_join.join(".*"),
            QRegularExpression::CaseInsensitiveOption);
    }

    void setFilter(const QString & keyword) {
        static QRegExp emptyRegExp;

        emit layoutAboutToBeChanged();

        startFiltering(keyword);
    }

    void startFiltering(const QString keyword) {
        auto future = QtConcurrent::run(this, &MyFilter::update_filter, keyword);

        watcher_->cancel();
        watcher_->setFuture(future);
    }

    void filteringDone() {
        emit layoutChanged();
    }

    bool update_filter(const QString & keyword) {
        long count = 0;
        auto expression = genRegexp(keyword);

        // TODO: do chunk-wise item insertion
        for (long i = 0; i < items_.size(); i++) {
            if (filterAcceptsRow(keyword, expression, i)) {
                shown_items_temp_[count++] = i;
            }
        }

        if (keyword.size())
            std::stable_sort(shown_items_temp_.begin(), shown_items_temp_.begin() + count, [=](int lhs, int rhs) -> bool {
            return false;
            //const Action *lhs_ = &items_.at(lhs);
            //const Action *rhs_ = &items_.at(rhs);
            //return distance(keyword, (lhs_->description())) < distance(keyword, (rhs_->description()));
                });

        shown_items_ = shown_items_temp_;

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
