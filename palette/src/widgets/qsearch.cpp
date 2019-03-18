#include <widgets/myfilter.h>
#include <widgets/qsearch.h>

void QSearch::onTextChanged() {
    const QString &keyword = text();

    entries_->setCurrentIndex(entries_->model()->index(0, 0));
    entries_->model()->setFilter(keyword);
}
