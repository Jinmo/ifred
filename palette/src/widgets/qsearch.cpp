#include <widgets/qsearch.h>

void QSearch::onTextChanged(const QString &keyword) {
    items_->scrollToTop();
    items_->model()->setFilter(keyword);
}
