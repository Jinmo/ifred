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

    class StylesObserver : public JSONObserver {
    public:
        StylesObserver(QPaletteInner* parent) : JSONObserver(parent, "theme/styles.json") {}

        void onUpdated(const QJsonObject& data) override {
            auto width = data["width"].toInt();
            auto parentWidget = static_cast<QPaletteInner*>(parent());

            if (!width) width = 750;
            parentWidget->resize(width, 0);
        }

    } *styles_observer_;

protected:
    QItems* entries_;
    QVBoxLayout* layout_;
    QSearch* searchbox_;

    QString name_;

    CSSObserver* css_observer_;

public:
    QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items);

    QSearch& searchbox() { return *searchbox_; }

    QItems& entries() { return *entries_; }

    void processEnterResult(bool res);

    bool onArrowPressed(int key);

    bool eventFilter(QObject* obj, QEvent* event) override;

    void keyPressEvent(QKeyEvent* e) override;

    void closeWindow();

    bool onEnterPressed();

    void onItemClicked(const QModelIndex& index);

    QPaletteContainer* container();

signals:
    bool enter_callback(Action& action);
};
