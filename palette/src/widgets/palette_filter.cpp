#include <widgets/palette_filter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

typedef QHash<QPair<QString, QString>, int> DistanceHash;
#define SAME_THREAD_THRESHOLD 20000

int distance(const QString& s1_, const QString& s2_) {
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


void PaletteFilter::setFilter(const QString & keyword) {
    keyword_ = keyword;
    search_service_->cancel();
    emit search_service_->startSearching(keyword);
}

void PaletteFilter::onDoneSearching(std::vector<int> * indexes_, int count, int preferred_index) {
    emit layoutAboutToBeChanged();
    std::copy(indexes_->begin(), indexes_->begin() + count, shown_items_.begin());
    shown_items_count_ = count;

    emit layoutChanged();
    emit filteringDone(preferred_index);
}

QModelIndex PaletteFilter::index(int row, int column, const QModelIndex & parent) const {
    return createIndex(row, column);
}

QVariant PaletteFilter::data(const QModelIndex & index, int role) const {
    if (role == Qt::DisplayRole)
        return QVariant::fromValue(search_service_->actions()[shown_items_[index.row()]]);
    else if (role == Qt::UserRole)
        return keyword_;
    return QVariant();
}

int PaletteFilter::rowCount(const QModelIndex & parent) const {
    return shown_items_count_;
}

PaletteFilter::PaletteFilter(QWidget * parent, const QString & palette_name, const QVector <Action> & items)
    : QAbstractItemModel(parent), shown_items_count_(0), shown_items_(items.size()) {

    search_service_ = new SearchService(nullptr, palette_name, items);
    connect(search_service_, &SearchService::doneSearching, this, &PaletteFilter::onDoneSearching);

    setFilter(QString());

    // In my opinion, total character count can be checked either.
    if (items.count() >= SAME_THREAD_THRESHOLD) {
        searcher_ = new QThread(this);
        searcher_->start();
        search_service_->moveToThread(searcher_);
    }
    else {
        searcher_ = nullptr;
        search_service_->setParent(this);
    }
}

bool SearchService::match(const QString & keyword, Action & action) {
    if (keyword.isEmpty())
        return true;

    return fts::fuzzy_match_simple(keyword, action.name);
}

void SearchService::search(const QString & keyword) {
    long count = 0, preferred_index = -1;
    QHash<QString, int> recent_actions(recent_actions_);

    canceled_ = false;

    /* Filter the items with fuzzy matching: see PaletteFilter::fuzzy_match_simple,
       which is substr with non-neighbor characters support
    */
    for (long i = 0; i < indexes_.size(); i++) {
        if (canceled_)
            return;
        auto r = recent_actions.find(actions_[i].id);
        if (match(keyword, actions_[i])) {
            indexes_[count++] = i;
            if (r != recent_actions.end()) {
                preferred_index = preferred_index + 1;
            }
        }
    }

    /* Sort by fuzzy matching if keyword is longer than 1 character
       This sorts indexes_ which is array of indexes, and copies to shown_items_.
       The size/capacity of each vector is not changed, just member variable shown_items_count_ is changed.

       If the job is cancelled during sort, the compare function raises exception to abort sorting.
    */
    try {
        std::sort(indexes_.begin(), indexes_.begin() + count, [=](int lhs, int rhs) -> bool {
            if (canceled_)
                throw std::exception();

            auto lhs_r = recent_actions.find(actions_[lhs].id);
            auto rhs_r = recent_actions.find(actions_[rhs].id);

            if (lhs_r == recent_actions.end() && rhs_r != recent_actions.end())
                return false;
            else if (rhs_r == recent_actions.end() && lhs_r != recent_actions.end())
                return true;
            else if (lhs_r != recent_actions.end() && rhs_r != recent_actions.end())
                return *lhs_r < *rhs_r;

            if (keyword.size() > 1)
                return distance(keyword, actions_[lhs].name) < distance(keyword, actions_[rhs].name);
            else
                return lhs < rhs;
            });
    }
    catch (std::exception &) {
        return;
    }

    emit doneSearching(&indexes_, count, preferred_index);
}

QHash<QString, int> fromVariant(const QHash<QString, QVariant> & source) {
    QHash<QString, int> result;
    for (auto&& it : source.keys()) {
        result[it] = source[it].toInt();
    }
    return result;
}

QHash<QString, QVariant> toVariant(const QHash<QString, int> & source) {
    QHash<QString, QVariant> result;
    for (auto&& it : source.keys()) {
        result[it] = source[it];
    }
    return result;
}

SearchService::SearchService(QObject * parent, const QString & palette_name, const QVector <Action> & actions) : QObject(parent), actions_(actions), storage_("ifred", palette_name), indexes_(actions.size()), canceled_(false) {
    connect(this, &SearchService::startSearching, this, &SearchService::search);
    connect(this, &SearchService::reportAction, [=](const QString & id) {
        for (auto&& it : recent_actions_.keys()) {
            recent_actions_[it]++;
        }
        recent_actions_[id] = 0;
        storage_.setValue("recent_actions", toVariant(recent_actions_));
        storage_.sync();
        });

    storage_.sync();
    recent_actions_ = fromVariant(storage_.value("recent_actions").toHash());
}
