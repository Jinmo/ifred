#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qitems.h"
#include "qsearch.h"

#include "common_defs.h"

#include <ida.hpp>
#include <idp.hpp>

class QPaletteInner;

struct EnterResult
{
    bool hide_;
    QPaletteInner *nextPalette_;

public:
    EnterResult(bool hide) : hide_(hide), nextPalette_(nullptr) {}
    EnterResult(QPaletteInner *nextPalette) : hide_(true), nextPalette_(nextPalette) {}

    bool hide() { return hide_; }
    auto *nextPalette() { return nextPalette_; }
};

class QPaletteInner : public QFrame
{
protected:
    QMainWindow *mainWindow_;
    QItems entries_;
    QVBoxLayout layout_;
    QSearch searchbox_;

    class ShadowObserver : public QConfigObserver
    {
        QPaletteInner *owner_;

    public:
        ShadowObserver(QPaletteInner *owner) : owner_(owner), QConfigObserver("styles.json") {}

        void onConfigUpdated(QJsonObject &config) override
        {
            int kShadow = config["shadow"].toInt();

            auto *effect = new QGraphicsDropShadowEffect();

            effect->setBlurRadius(kShadow);
            effect->setColor(QColor(0, 0, 0, 100));
            effect->setOffset(0);

            owner_->setGraphicsEffect(effect);

            auto mainWindow = owner_->mainWindow();
            if(mainWindow)
                mainWindow->setContentsMargins(kShadow, kShadow, kShadow, kShadow);
        }

    } shadow_observer_;

public:
    auto &searchbox() { return searchbox_; }
    auto &entries() { return entries_; }
    QMainWindow *mainWindow() { return mainWindow_; }

    QPaletteInner()
    : mainWindow_(nullptr), entries_(), searchbox_(this, &entries_.model()), shadow_observer_(this)
    {
        setWindowFlags(Qt::FramelessWindowHint);
        // setAttribute(Qt::WA_TranslucentBackground);

        // TODO: we need to repaint the list item... I don't know how.
        connect(&entries_.model(), &MyFilter::dataChanged, &entries_, [=]() { entries_.viewport()->update(); });

        layout_.addWidget(&searchbox_);
        layout_.addWidget(&entries_);
        layout_.setContentsMargins(0, 0, 0, 0);
        layout_.setSpacing(0);

        setLayout(&layout_);

        resize(WIDTH, HEIGHT);

        CSSLOADER("window.css");

        connect(&searchbox_, &QSearch::returnPressed, this, &QPaletteInner::onEnterPressed);
        connect(&searchbox_, &QSearch::textChanged, this, &QPaletteInner::onTextChanged);

        shadow_observer_.activate();
    }

    void processEnterResult(EnterResult res)
    {
        if (res.hide())
        {
            if (res.nextPalette())
            {
                QPaletteInner *p = res.nextPalette();
                p->setParent(mainWindow_);
                hide();
            }
            else
            {
                mainWindow_->hide();
            }
        }
        else
        {
            // hide=true if nextPalette != NULL
            assert(!res.nextPalette());
        }
    }

    void onEnterPressed()
    {
        auto res = enter_callback();
        processEnterResult(res);
    }

    void setParent(QMainWindow *parent)
    {
        QFrame::setParent(parent);
        parent->installEventFilter(this);

        mainWindow_ = parent;
        mainWindow_->setCentralWidget(this);

        shadow_observer_.updated();
    }

    void onTextChanged(const QString &)
    {
        entries_.setCurrentIndex(entries_.model().index(0, 0));
        entries_.scrollToTop();
        entries_.repaint();
    }

    bool arrow_callback(int key)
    {
        int delta;
        if (key == Qt::Key_Down)
        {
            delta = 1;
        }
        else
        {
            delta = -1;
        }
        auto new_row = entries_.currentIndex().row() + delta;
        if (new_row == -1)
            new_row = 0;
        else if (new_row == entries_.model().rowCount())
            new_row = 0;
        entries_.setCurrentIndex(entries_.model().index(new_row, 0));
        return true;
    }

    virtual EnterResult enter_callback() = 0;

    bool eventFilter(QObject *obj, QEvent *event) override
    {
        // if(obj != &searchbox_ && obj != &entries_) return QFrame::eventFilter(obj, event);
        switch (event->type())
        {
            case QEvent::KeyPress:
            {
                QKeyEvent *ke = static_cast<QKeyEvent *>(event);
                switch (ke->key())
                {
                    case Qt::Key_Down:
                    case Qt::Key_Up:
                    return arrow_callback(ke->key());
                    default:
                    return QFrame::eventFilter(obj, event);
                }
            }
            default:
            return QFrame::eventFilter(obj, event);
        }
    }

    void keyPressEvent(QKeyEvent *e) override
    {
        if (e->key() != Qt::Key_Escape)
            QFrame::keyPressEvent(e);
        else
        {
            if (mainWindow_)
                mainWindow_->hide();
        }
        shadow_observer_.updated();
    }
};
