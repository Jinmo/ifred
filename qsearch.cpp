#include "qsearch.h"
#include "common_defs.h"

char highlightTable[65536]; // not emoji!
QSet<QString> g_search;

void QSearch::onTextChanged()
{
    QString &keyword = text();
    memset(&highlightTable, 0, sizeof(highlightTable));
    for (auto &i : keyword)
    {
        ushort c = i.toLower().unicode();
        highlightTable[c]++;
    }
    filter_->setMyFilter(keyword);
    filter_->sort(0);
}
