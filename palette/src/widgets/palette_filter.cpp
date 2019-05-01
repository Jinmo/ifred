#include <widgets/palette_filter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

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

bool PaletteFilter::lessThan(const QString & keyword, Action & left, Action &right) const {
    return distance(keyword, left.description()) < distance(keyword, right.description());
}

void PaletteFilter::setFilter(const QString &keyword) {
    long count = 0, preferred_index = 0;

    keyword_ = keyword;
    emit layoutAboutToBeChanged();

    /* Filter the items with fuzzy matching: see PaletteFilter::fuzzy_match_simple,
       which is substr with non-neighbor characters support
    */
    for (long i = 0; i < items_.size(); i++) {
        if (match(keyword, items_[i])) {
            shown_items_temp_[count++] = i;
        }
    }

    /* Sort by fuzzy matching if keyword is longer than 1 character
       This sorts shown_items_temp_ which is array of indexes, and copies to shown_items_.
       The size/capacity of each vector is not changed, just member variable shown_items_count_ is changed.

       If the job is cancelled during sort, the compare function raises exception to abort sorting.

       TODO: set preferred_index_ from command history to focus on the recently executed action
    */
    if (keyword.size() > 1)
        std::sort(shown_items_temp_.begin(), shown_items_temp_.begin() + count, [=](int lhs, int rhs) -> bool {
            Action& l = items_[lhs];
            Action& r = items_[rhs];
            return lessThan(keyword_, l, r);
            });

    std::copy(shown_items_temp_.begin(), shown_items_temp_.begin() + count, shown_items_.begin());
    shown_items_ = shown_items_temp_;
    shown_items_count_ = count;
    preferred_index_ = preferred_index;

    emit filteringDone(preferred_index_);
    emit layoutChanged();
}

QModelIndex PaletteFilter::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column);
}

QVariant PaletteFilter::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole)
        return QVariant::fromValue(items_[shown_items_[index.row()]]);
    else if (role == Qt::UserRole)
        return keyword_;
    return QVariant();
}

int PaletteFilter::rowCount(const QModelIndex &parent) const {
    return shown_items_count_;
}

bool PaletteFilter::match(const QString &keyword, Action &action) {
    if (keyword.isEmpty())
        return true;

    return fts::fuzzy_match_simple(keyword, action.description());
}

PaletteFilter::PaletteFilter(QWidget* parent, const QVector <Action>& items)
    : QAbstractItemModel(parent), items_(items), shown_items_count_(0), preferred_index_(0),
    shown_items_(items.size()), shown_items_temp_(items.size()) {
    setFilter(QString());
}
