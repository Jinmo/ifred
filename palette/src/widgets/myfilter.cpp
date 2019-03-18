#include <widgets/myfilter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

typedef QHash<QPair<QString, QString>, int> DistanceHash;
QThreadStorage<DistanceHash*> distances;
int distance(const QString& s1_, const QString& s2_) {

    QString s1 = s1_.toLower();
    QPair<QString, QString> pair(s1, s2_.toLower());

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

bool MyFilter::lessThan(const QModelIndex & left, const QModelIndex & right) const {
#define FUZZY 0

#if !FUZZY
    return false;
#else
    const QString& filterText = g_keyword;
    const QString& leftData = sourceModel()->data(left).toString(),
        & rightData = sourceModel()->data(right).toString();

    if (filterText.size() < 2)
        return leftData < rightData;

    return distance(filterText, leftData) < distance(filterText, rightData);
#endif
}

QString g_keyword;

void prepareHighlighter(const QString &str) {
    g_keyword = str;
}

