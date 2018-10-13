#ifndef QSEARCH_H
#define QSEARCH_H


#include <QtGui>
#include <QtWidgets>
#include <functional>

#include "common_defs.h"
#include "myfilter.h"

#include <idp.hpp>

class QSearch: public QLineEdit, public FredCallback {
    MyFilter *filter_;
public:
    QSearch(QWidget *parent, MyFilter *filter): QLineEdit(parent), filter_(filter) {
        setStyleSheet(R"(
                         QLineEdit, QLineEdit:hover, QLineEdit:active {
                             font-size: 27px;
                             width: 720px;
                             height: 63px;
                             border: none;
                         }
                         )");

        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);
    }

    void onTextChanged();

    void keyPressEvent(QKeyEvent *event) override {
        QLineEdit::keyPressEvent(event);
    }
};

#endif // QSEARCH_H
