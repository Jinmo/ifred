#include <widgets/myfilter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

QHash<QString, QRegularExpression> regexp_cache;
QHash<QString, QRegularExpression> capturing_regexp_cache;

QRegularExpression genRegexp(const QString& keyword) {
    QStringList regexp_before_join;

    if (regexp_cache.contains(keyword)) {
        return regexp_cache[keyword];
    }

    for (auto& x : keyword)
        if (!x.isSpace())
            regexp_before_join << x;

    QRegularExpression result(regexp_before_join.join(".*"),
        QRegularExpression::CaseInsensitiveOption);

    regexp_cache[keyword] = result;
    return result;
}

QRegularExpression genCapturingRegexp(const QString & keyword) {
    QStringList regexp_before_join;

    if (capturing_regexp_cache.contains(keyword)) {
        return capturing_regexp_cache[keyword];
    }

    regexp_before_join << ("^");

    for (auto& x : keyword)
        if (!x.isSpace())
            regexp_before_join << (QString("(.*)(") + x + ")");

    regexp_before_join.push_back("(.*)$");

    QRegularExpression result(regexp_before_join.join(""),
        QRegularExpression::CaseInsensitiveOption);

    capturing_regexp_cache[keyword] = result;
    return result;
}

typedef QHash<QPair<QString, QString>, int> DistanceHash;
QThreadStorage<DistanceHash*> distances;

int distance(const QString & s1_, const QString & s2_) {

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
    auto expression = genRegexp(keyword);

    canceled_ = false;

    // TODO: do chunk-wise item insertion
    for (long i = 0; i < items_.size(); i++) {
        if (filterAcceptsRow(keyword, expression, i)) {
            shown_items_temp_[count++] = i;
        }

        if (canceled_)
            return true;
    }

    if(keyword.size())
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

    shown_items_ = shown_items_temp_;
    shown_items_count_ = count;
    preferred_index_ = preferred_index;

    return true;
}
