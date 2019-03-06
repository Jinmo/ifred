#include "qsearch.h"
#include "common_defs.h"

QString g_keyword;

void prepareHighlighter(QString &str) {
    g_keyword = str;
}

void QSearch::onTextChanged()
{
    QString &keyword = text();

    prepareHighlighter(keyword);
    filter_->setMyFilter(keyword);
    filter_->sort(0);
}
