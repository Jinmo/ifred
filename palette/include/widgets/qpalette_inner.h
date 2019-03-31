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

class EnterResult {
    bool hide_;
    QVector<Action> nextPalette_;

public:
    EnterResult() : hide_(true), nextPalette_() {}
    EnterResult(bool hide) : hide_(hide), nextPalette_() {}
    EnterResult(const EnterResult& other) : hide_(other.hide_), nextPalette_(other.nextPalette_) {}

    EnterResult(const QVector<Action>& nextPalette) : hide_(true), nextPalette_(nextPalette) {}

    bool hide() { return hide_; }

    const QVector<Action>& nextPalette() { return nextPalette_; }
    EnterResult& operator =(const EnterResult& other) {
        hide_ = other.hide_;
        nextPalette_ = other.nextPalette_;

        return *this;
    }
};

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

    void processEnterResult(EnterResult res);

    bool onArrowPressed(int key);

    bool eventFilter(QObject* obj, QEvent* event) override;

    void keyPressEvent(QKeyEvent* e) override;

    void closeWindow();

    bool onEnterPressed() {
        if (entries_->model()->rowCount())
        {
            auto action = (entries_->currentIndex()).data().value<Action>();
            processEnterResult(true);
            emit enter_callback(action);
        }
        return true;
    }

    void onItemClicked(const QModelIndex& index);

    QPaletteContainer* container();

signals:
    bool enter_callback(Action& action);
};
