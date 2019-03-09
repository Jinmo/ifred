#include "myfilter.h"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

static QHash<QPair<QString, QString>, int> distances;

int distance(const QString &s1_, const QString &s2_) {
	QString s1 = s1_.toLower();
	QPair<QString, QString> pair(s1, s2_);

    if (distances.contains(pair))
        return distances[pair];

	QByteArray s1b = s1.toUtf8();
    QByteArray s2b = s2_.toUtf8();

    int score;
    fts::fuzzy_match(s1b.data(), s2b.data(), score);

    distances[pair] = -score;
    return -score;
}

bool MyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const {
#define FUZZY 0

#if !FUZZY
    return QSortFilterProxyModel::lessThan(left, right);
#else
    const QString &filterText = g_keyword;
    const QString &leftData = sourceModel()->data(left).toString(),
            &rightData = sourceModel()->data(right).toString();

    if (filterText.size() < 2)
        return leftData < rightData;

    return distance(filterText, leftData) < distance(filterText, rightData);
#endif
}
