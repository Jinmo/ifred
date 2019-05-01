#pragma once

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitems.h>
#include <widgets/qsearch.h>
#include <action.h>

#ifndef IMPORTING_PALETTE
#include "palette_export.h"
#else
#define PALETTE_EXPORT
#define PALETTE_IMPORT
#endif

class QPaletteInner;

class QPaletteContainer;

class PALETTE_EXPORT QPaletteInner : public QFrame {
    Q_OBJECT;

    QString name_;

    QItems* items_;
    QVBoxLayout* layout_;
    QSearch* searchbox_;

public:
    QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items);

    QSearch& searchbox() { return *searchbox_; }
    QItems& entries() { return *items_; }

    void processEnterResult(bool res);
    bool onArrowPressed(int key);
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    bool onEnterPressed();
    void onItemClicked(const QModelIndex& index);
    void showEvent(QShowEvent *event) override {
        searchbox_->selectAll();
        searchbox_->setFocus();
    }
signals:
    bool enter_callback(Action& action);
};
