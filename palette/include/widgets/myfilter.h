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

    QString keyword_;
    bool canceled_;

    MyFilter(QWidget* parent, const QVector<Action>& items)
        : QAbstractItemModel(parent), items_(std::move(items)), shown_items_count_(0), canceled_(false),
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

    QRegularExpression genRegexp(const QString& keyword) {
        QStringList regexp_before_join;

        for (auto& x : keyword)
            if (!x.isSpace())
                regexp_before_join.push_back(x);

        return QRegularExpression(regexp_before_join.join(".*"),
            QRegularExpression::CaseInsensitiveOption);
    }

    void setFilter(const QString & keyword) {
        keyword_ = keyword;
        emit layoutAboutToBeChanged();

        startFiltering(keyword);
    }

    void startFiltering(const QString keyword) {
        auto future = QtConcurrent::run(this, &MyFilter::update_filter, keyword);

        canceled_ = true;
        watcher_->setFuture(future);
    }

    void filteringDone() {
        emit layoutChanged();
    }

    bool update_filter(const QString & keyword) {
        long count = 0;
        auto expression = genRegexp(keyword);

        while(canceled_) QThread::sleep(0);
        canceled_ = false;

        // TODO: do chunk-wise item insertion
        for (long i = 0; i < items_.size(); i++) {
            if (filterAcceptsRow(keyword, expression, i)) {
                shown_items_temp_[count++] = i;
            }

            if (canceled_)
                return true;
        }

        shown_items_ = shown_items_temp_;
        shown_items_count_ = count;

        return true;
    }

    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override {
        return createIndex(row, column);
    };

    // we don't use this
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override {
        if(role == Qt::DisplayRole)
            return QVariant::fromValue(items_[shown_items_[index.row()]]);
        else
            return keyword_;
    }

    int	columnCount(const QModelIndex & parent) const override {
        return 1;
    }

    QModelIndex	parent(const QModelIndex & index) const override {
        return {};
    }

    int rowCount(const QModelIndex & parent = QModelIndex()) const override {
        return shown_items_count_;
    }
};

#endif // MYFILTER_H
