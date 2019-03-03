#include "qsearch.h"
#include "common_defs.h"

bool highlightTable[65536]; // not emoji!

void QSearch::onTextChanged()
{
    auto keyword = text();
    memset(&highlightTable, 0, sizeof(highlightTable));
    g_search.clear();
    if (keyword.size())
        g_search.unite(g_intern[keyword[0].toLower().unicode()]);
    for (auto &i : keyword)
    {
        ushort c = i.toLower().unicode();
        if (!highlightTable[c])
        {
            if (g_intern.contains(c))
                g_search.intersect(g_intern[c]);
            else
                g_search.clear();
        }
        highlightTable[c] = true;
    }
    filter_->setFilter(keyword);
    filter_->sort(0);
}
