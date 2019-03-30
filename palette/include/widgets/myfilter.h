#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

int distance(const QString& s1_, const QString& s2_);
QRegularExpression genRegexp(const QString& keyword);
QRegularExpression genCapturingRegexp(const QString& keyword);

class MyFilter : public QAbstractItemModel {
    Q_OBJECT;
public:
    const QVector<Action> items_;
    QVector<int> shown_items_;
    QVector<int> shown_items_temp_;

    QVector<int> initial_range_;
    int shown_items_count_;

    QFutureWatcher<bool>* watcher_;
    QFuture<bool> future_;

    QString keyword_;
    bool canceled_;

    long preferred_index_;

    MyFilter(QWidget* parent, const QVector<Action>& items)
        : QAbstractItemModel(parent), items_(std::move(items)), shown_items_count_(0), preferred_index_(0), canceled_(false),
        shown_items_(items.size()), shown_items_temp_(items.size()), future_(), watcher_(new QFutureWatcher<bool>()),
        initial_range_(items.size()) {
        setFilter(QString());

        for (int i = 0; i < items.size(); i++) {
            initial_range_[i] = i;
        }

        connect(watcher_, &QFutureWatcher<bool>::finished, this, &MyFilter::doneFiltering);
    }

    // Public interface
    static bool fuzzy_match_simple(const QString &pattern, const QString &str) {
        auto it = pattern.begin();
        auto itEnd = pattern.end();

        if (it == itEnd)
            return true;

        for (auto&& c : str) {
            if (it->toLower() == c.toLower()) {
                ++it;
                if (it == itEnd) {
                    break;
                }
            }
        }

        return it == itEnd;
    }

    bool filterAcceptsRow(const QString& keyword, QRegularExpression regexp, int source_row) {
        if (keyword.isEmpty())
            return true;

        const QString& str = items_[source_row].description();
        return fuzzy_match_simple(keyword, str);
        bool result = str.contains(regexp);
        return result;
    }

    bool lessThan(const QString& keyword, const QString& left, const QString& right) const;

    void setFilter(const QString& keyword) {
        keyword_ = keyword;
        emit layoutAboutToBeChanged();

        canceled_ = true;

        auto future = QtConcurrent::run(this, &MyFilter::update_filter, keyword);
        future.waitForFinished();
        doneFiltering();
    }

    void doneFiltering() {
        emit filteringDone(preferred_index_);
        emit layoutChanged();
    }

    bool update_filter(const QString& keyword);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override {
        return createIndex(row, column);
    };

    // we don't use this
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (role == Qt::DisplayRole)
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
signals:
    void filteringDone(int preferred_index);
};

#endif // MYFILTER_H
