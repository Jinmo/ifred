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

struct EnterResult {
    bool hide_;
    QPaletteInner* nextPalette_;

public:
    EnterResult(bool hide) : hide_(hide), nextPalette_(nullptr) {}

    EnterResult(QPaletteInner* nextPalette) : hide_(true), nextPalette_(nextPalette) {}

    bool hide() { return hide_; }

    QPaletteInner* nextPalette() { return nextPalette_; }
};

class QPaletteContainer;

class PALETTE_EXPORT QPaletteInner : public QFrame {
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
    QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items);

public:
    QSearch& searchbox() { return *searchbox_; }

    QItems& entries() { return *entries_; }

    void processEnterResult(EnterResult res);

    bool onArrowPressed(int key);

    virtual EnterResult enter_callback(Action & action) = 0;

    bool eventFilter(QObject * obj, QEvent * event) override;

    void keyPressEvent(QKeyEvent * e) override;

    void closeWindow();

    void onEnterPressed() {
        auto action = (entries_->currentIndex()).data().value<Action>();
        auto res = enter_callback(action);
        processEnterResult(res);
    }

    void onItemClicked(const QModelIndex & index);

    QPaletteContainer *container();
};
