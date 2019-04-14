#include <widgets/myfilter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

static bool fuzzy_match_simple(const QString& pattern, const QString& str) {
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

typedef QHash<QPair<QString, QString>, int> DistanceHash;

int distance(const QString & s1_, const QString & s2_) {
    static QThreadStorage<DistanceHash*> distances;

    QString s1 = s1_.toLower();
    QPair<QString, QString> pair(s1, s2_);

    if (!distances.hasLocalData()) {
        distances.setLocalData(new DistanceHash());
    }

    if (distances.localData()->contains(pair))
        return (*distances.localData())[pair];

    QByteArray s1b = s1.toUtf8();
    QByteArray s2b = s2_.toUtf8();

    int score;
    fts::fuzzy_match(s1b.data(), s2b.data(), score);

    distances.localData()->insert(pair, -score);
    return -score;
}

bool MyFilter::lessThan(const QString & keyword, const QString & left, const QString & right) const {
    return distance(keyword, left) < distance(keyword, right);
}

bool MyFilter::update_filter(const QString & keyword) {
    long count = 0, preferred_index = 0;

    canceled_ = false;

    /* Filter the items with fuzzy matching: see MyFilter::fuzzy_match_simple,
       which is substr with non-neighbor characters support

       TODO: do chunk-wise item insertion
    */
    for (long i = 0; i < items_.size(); i++) {
        if (filterAcceptsRow(keyword, i)) {
            shown_items_temp_[count++] = i;
        }

        if (canceled_)
            return true;
    }

    /* Sort by fuzzy matching if keyword is longer than 1 character
       This sorts shown_items_temp_ which is array of indexes, and copies to shown_items_.
       The size/capacity of each vector is not changed, just member variable shown_items_count_ is changed.

       If the job is cancelled during sort, the compare function raises exception to abort sorting.

       TODO: set preferred_index_ from command history to focus on the recently executed action
    */
    if (keyword.size() > 1)
        try {
        std::sort(shown_items_temp_.begin(), shown_items_temp_.begin() + count, [=](int lhs, int rhs) -> bool {
            if (canceled_)
                throw std::exception();
            auto&& l = items_[lhs];
            auto&& r = items_[rhs];
            return lessThan(keyword_, l.description(), r.description());
            });
    }
    catch (std::exception&) {
        return true;
    }

    std::copy(shown_items_temp_.begin(), shown_items_temp_.begin() + count, shown_items_.begin());
    shown_items_ = shown_items_temp_;
    shown_items_count_ = count;
    preferred_index_ = preferred_index;

    return true;
}

void MyFilter::setFilter(const QString &keyword) {
    keyword_ = keyword;
    emit layoutAboutToBeChanged();

    canceled_ = true;

    auto future = QtConcurrent::run(this, &MyFilter::update_filter, keyword);
    future.waitForFinished();
    doneFiltering();
}

void MyFilter::doneFiltering() {
    emit filteringDone(preferred_index_);
    emit layoutChanged();
}

QModelIndex MyFilter::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column);
}

QVariant MyFilter::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
        return QVariant::fromValue(items_[shown_items_[index.row()]]);
    else
        return keyword_;
}

int MyFilter::rowCount(const QModelIndex &parent) const {
    return shown_items_count_;
}

MyFilter::MyFilter(QWidget *parent, const QVector <Action> &items)
        : QAbstractItemModel(parent), items_(items), shown_items_count_(0), preferred_index_(0), canceled_(false),
          shown_items_(items.size()), shown_items_temp_(items.size()), future_(), watcher_(new QFutureWatcher<bool>()) {
    setFilter(QString());

    connect(watcher_, &QFutureWatcher<bool>::finished, this, &MyFilter::doneFiltering);
}

bool MyFilter::filterAcceptsRow(const QString &keyword, int source_row) {
    if (keyword.isEmpty())
        return true;

    return fuzzy_match_simple(keyword, items_[source_row].description());
}
