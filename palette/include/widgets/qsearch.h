#ifndef QSEARCH_H
#define QSEARCH_H

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitems.h>
#include "observers.h"

class QSearch : public QLineEdit {
    QItems* items_;

    void onTextChanged(const QString& keyword);

public:
    QSearch(QWidget* parent, QItems* items)
        : QLineEdit(parent), items_(items) {
        connect(this, &QLineEdit::textChanged, this, &QSearch::onTextChanged);
        onTextChanged(QString());
    }
};

#endif // QSEARCH_H
