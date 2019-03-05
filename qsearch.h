#ifndef QSEARCH_H
#define QSEARCH_H

#include <QtGui>
#include <QtWidgets>
#include <functional>

#include "common_defs.h"
#include "myfilter.h"

class QSearch : public QLineEdit, public FredCallback
{
    MyFilter *filter_;

  public:
    QSearch(QWidget *parent, MyFilter *filter) : QLineEdit(parent), filter_(filter)
    {
        CSSLOADER("searchbox.css");
        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);

        onTextChanged();
    }

    void onTextChanged();

    void keyPressEvent(QKeyEvent *event) override
    {
        QLineEdit::keyPressEvent(event);
    }
};

#endif // QSEARCH_H
