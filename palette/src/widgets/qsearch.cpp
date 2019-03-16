#include <widgets/myfilter.h>

#include "widgets/qsearch.h"
#include "common_defs.h"

QString g_keyword;

void prepareHighlighter(QString &str) {
    g_keyword = str;
}

void QSearch::onTextChanged() {
    QString &keyword = text();

    prepareHighlighter(keyword);
    entries_->model()->setFilter(keyword);
}
