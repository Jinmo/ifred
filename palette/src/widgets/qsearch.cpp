#include <widgets/myfilter.h>
#include <widgets/qsearch.h>

void QSearch::onTextChanged(const QString &keyword) {
    entries_->scrollToTop();
    entries_->model()->setFilter(keyword);
}
