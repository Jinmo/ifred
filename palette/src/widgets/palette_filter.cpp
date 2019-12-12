#include <widgets/palette_filter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

constexpr int SAME_THREAD_THRESHOLD = 20000;
constexpr int MAX_RECENT_ITEMS = 100;

using DistanceHash = QHash<QPair<QString, QString>, int>;

class canceled_error : public std::exception
{
};

int distance(const QString &s1_, const QString &s2_)
{
    static QThreadStorage<DistanceHash *> distances;

    QString s1 = s1_.toLower();
    QPair<QString, QString> pair(s1, s2_);

    if (!distances.hasLocalData())
    {
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

void PaletteFilter::setFilter(const QString &keyword)
{
    search_service_->search(keyword);
}

void PaletteFilter::onDoneSearching(QString keyword, QVector<Action> items, int recent_count)
{
    emit layoutAboutToBeChanged();
    shown_items_ = items;
    keyword_ = keyword;
    emit layoutChanged();
    emit filteringDone(recent_count);
}

QModelIndex PaletteFilter::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QVariant PaletteFilter::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QVariant::fromValue(shown_items_[index.row()]);
    else if (role == Qt::UserRole)
        return keyword_;
    return QVariant();
}

int PaletteFilter::rowCount(const QModelIndex &parent) const
{
    return shown_items_.count();
}

PaletteFilter::PaletteFilter(QWidget *parent, const QString &palette_name, const QVector<Action> &items, SearchService *search_service)
    : QAbstractItemModel(parent), shown_items_(), search_service_(search_service)
{
    // In my opinion, total character count can be checked either.
    if (items.count() >= SAME_THREAD_THRESHOLD)
    {
        searcher_ = new QThread(this);
        searcher_->start();
        search_service_->moveToThread(searcher_);
    }
    else
    {
        searcher_ = nullptr;
        search_service_->setParent(this);
    }

    connect(search_service_, &SearchService::doneSearching, this, &PaletteFilter::onDoneSearching, Qt::QueuedConnection);
}

void BasicService::search(const QString &keyword)
{
    long nonrecent_count = 0, recent_count = 0;
    QHash<QString, int> recent_actions(recent_actions_);

    canceled_ = false;

    /* Filter the items with fuzzy matching: see PaletteFilter::fuzzy_match_simple,
       which is substr with non-neighbor characters support
    */
    for (long i = 0; i < indexes_.size(); i++)
    {
        if (canceled_)
            return;
        if (keyword.isEmpty() || fts::fuzzy_match_simple(keyword, actions_[i].name))
        {
            if (recent_actions.count(actions_[i].id))
            {
                recent_indexes_[recent_count++] = i;
            }
            else
            {
                indexes_[nonrecent_count++] = i;
            }
        }
    }

    try
    {
        /* Sort by how recent the item is.
           If the job is canceled during sort, the compare function raises exception to abort sorting.
        */
        std::sort(recent_indexes_.begin(), recent_indexes_.begin() + recent_count, [=](int lhs, int rhs) -> bool {
            if (canceled_)
                throw canceled_error();
            auto lhs_r = recent_actions.find(actions_[lhs].id);
            auto rhs_r = recent_actions.find(actions_[rhs].id);

            return *lhs_r < *rhs_r;
        });

        /* Sort by fuzzy matching if keyword is longer than 1 character */
        if (keyword.size() > 1)
            std::sort(indexes_.begin(), indexes_.begin() + nonrecent_count, [=](int lhs, int rhs) -> bool {
            if (canceled_)
                throw canceled_error();
            return distance(keyword, actions_[lhs].name) < distance(keyword, actions_[rhs].name);
            });
    }
    catch (canceled_error &)
    {
        return;
    }

    QVector<Action> result;
    result.reserve(recent_count + nonrecent_count);

    for (int i = 0; i < recent_count; i++)
    {
        result.push_back(actions_[recent_indexes_[i]]);
    }

    for (int i = 0; i < nonrecent_count; i++)
    {
        result.push_back(actions_[indexes_[i]]);
    }

    emit doneSearching(keyword, result, recent_count);
}

static int convert(QVariant a) { return a.toInt(); }

static int convert(int a) { return a; }

template <typename A, typename B>
QHash<QString, A> convert(const QHash<QString, B> &source)
{
    QHash<QString, A> result;
    for (auto &&it : source.keys())
    {
        result[it] = convert(source[it]);
    }
    return result;
}

BasicService::BasicService(QObject *parent, const QString &palette_name, const QVector<Action> &actions)
    : SearchService(parent), actions_(actions), storage_("ifred", palette_name), indexes_(actions.size()), recent_indexes_(), canceled_(false)
{
    connect(this, &SearchService::startSearching, this, &BasicService::search);
    connect(this, &SearchService::itemClicked, [=](const QString &id) {
        QVector<QString> to_remove;
        for (auto &&it : recent_actions_.keys())
        {
            if (++recent_actions_[it] >= MAX_RECENT_ITEMS)
            {
                to_remove.push_back(it);
            }
        }
        for (auto &&key : to_remove)
        {
            recent_actions_.remove(key);
        }
        recent_actions_[id] = 0;
        storage_.setValue("recent_actions", convert<QVariant, int>(recent_actions_));
        storage_.sync(); // save to platform-specific registry immediately
    });

    storage_.sync();
    recent_actions_ = convert<int, QVariant>(storage_.value("recent_actions").toHash());
    recent_indexes_.resize(recent_actions_.size());
}
