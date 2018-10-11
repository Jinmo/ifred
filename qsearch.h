#ifndef QSEARCH_H
#define QSEARCH_H


#include <QtGui>
#include <QtWidgets>

#include "myfilter.h"

class QSearch: public QLineEdit {
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

    bool event(QEvent *event) {
        switch(event->type()) {
        case QEvent::KeyPress: {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            switch (ke->key()) {
            case Qt::Key_Down:
                return true;
            case Qt::Key_Up:
                return true;
            default:
                return QLineEdit::event(event);
            }
        }
        default:
            return QLineEdit::event(event);
        }
    }
};

#endif // QSEARCH_H
