#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qcommands.h"
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
    QCommands commands_;
    QVBoxLayout layout_;
    QSearch searchbox_;

  public:
    auto &searchbox() { return searchbox_; }

    QPaletteInner() : mainWindow_(nullptr), commands_(), searchbox_(this, &commands_.model())
    {
        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);

        // TODO: we need to repaint the list item... I don't know how.
        connect(&commands_.model(), &MyFilter::dataChanged, &commands_, [=]() { commands_.viewport()->update(); });

        layout_.addWidget(&searchbox_);
        layout_.addWidget(&commands_);
        layout_.setContentsMargins(0, 0, 0, 0);
        layout_.setSpacing(0);

        setLayout(&layout_);

        resize(WIDTH, HEIGHT);

        CSSLOADER("window.css");

        connect(&searchbox_, &QSearch::returnPressed, this, &QPaletteInner::onEnterPressed);
        connect(&searchbox_, &QSearch::textChanged, this, &QPaletteInner::onTextChanged);
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
    }

    void onTextChanged(const QString &)
    {
        commands_.setCurrentIndex(commands_.model().index(0, 0));
        commands_.scrollToTop();
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
        auto new_row = commands_.currentIndex().row() + delta;
        if (new_row == -1)
            new_row = 0;
        commands_.setCurrentIndex(commands_.model().index(new_row, 0));
        return true;
    }

    virtual EnterResult enter_callback() = 0;

    bool eventFilter(QObject *obj, QEvent *event) override
    {
        // if(obj != &searchbox_ && obj != &commands_) return QFrame::eventFilter(obj, event);
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
    }
};
