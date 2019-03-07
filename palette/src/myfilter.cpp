#include "myfilter.h"
#include "common_defs.h"

#define FTS_FUZZY_MATCH_IMPLEMENTATION

#include "fts_fuzzy_match.h"

static QHash<QString, QHash<QString, int>> distances;

int distance(const QString &s1_, const QString &s2_) {
    QString s1 = s1_.toLower();
    QString s2 = s2_.toLower();

    if (distances.contains(s1) && distances[s1].contains(s2))
        return distances[s1][s2];

    QByteArray s1b = s1.toUtf8();
    QByteArray s2b = s2.toUtf8();

    int score;
    fts::fuzzy_match(s1b.data(), s2b.data(), score);

    distances[s1][s2] = -score;
    return -score;
}

bool MyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
#define FUZZY 1

#if !FUZZY
    return QSortFilterProxyModel::lessThan(left, right);
#else
    const QString &filterText = g_keyword;
    const QString &leftData = sourceModel()->data(left).toString(),
            &rightData = sourceModel()->data(right).toString();

    if (filterText.size() == 0)
        return leftData < rightData;

    return distance(filterText, leftData) < distance(filterText, rightData);
#endif
}
