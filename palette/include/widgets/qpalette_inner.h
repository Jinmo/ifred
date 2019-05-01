#pragma once

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitems.h>
#include <action.h>

#include "palette_export.h"

class QItems;

class PALETTE_EXPORT QPaletteInner : public QFrame {
    Q_OBJECT;

    QString name_;

    QLineEdit* searchbox_;
    QItems* items_;

    bool onEnterPressed();
    void onItemClicked(const QModelIndex& index);

    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override {
        emit closed();
    }
public:
    QPaletteInner(QWidget* parent, const QString &name, const QVector<Action>& items);

    void setPlaceholderText(const QString &placeholder);
signals:
    bool itemClicked(Action &action);
    void closed();
};
