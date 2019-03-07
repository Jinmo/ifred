#ifndef QSEARCH_H
#define QSEARCH_H

#include <QtGui>
#include <QtWidgets>

#include "myfilter.h"
#include "cssobserver.h"

class QSearch : public QLineEdit {
    MyFilter *filter_;
    CSSObserver *css_observer_;

public:
    QSearch(QWidget *parent, MyFilter *filter)
            : QLineEdit(parent), filter_(filter),
              css_observer_(new CSSObserver(this, "searchbox.css")) {
        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);
        onTextChanged();
    }

    void onTextChanged();

    void keyPressEvent(QKeyEvent *event) override {
        QLineEdit::keyPressEvent(event);
    }
};

#endif // QSEARCH_H
