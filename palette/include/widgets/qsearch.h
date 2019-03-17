#ifndef QSEARCH_H
#define QSEARCH_H

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitems.h>
#include "cssobserver.h"

class QSearch : public QLineEdit {
    QItems* entries_;
    CSSObserver* css_observer_;

public:
    QSearch(QWidget* parent, QItems* entries)
        : QLineEdit(parent), entries_(entries),
        css_observer_(new CSSObserver(this, "theme/searchbox.css")) {
        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);
        onTextChanged();
    }

    void onTextChanged();
};

#endif // QSEARCH_H
