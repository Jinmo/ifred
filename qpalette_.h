#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qpalette_inner.h"
#include "utils.h"

static void centerWidgets(QWidget *widget, QWidget *host = nullptr)
{
    if (!host)
        host = widget->parentWidget();

    if (host)
    {
        auto hostRect = host->geometry();
        widget->move(hostRect.center() - widget->rect().center());
    }
    else
    {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }
}

template <class T>
class QPalette_ : public QMainWindow
{
    T *inner_;
    QGraphicsDropShadowEffect *effect_;

  public:
    QPaletteInner &inner() { return *inner_; }
    QPalette_(): inner_(new T())
    {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

        inner().setParent(this);
    }

    auto &effect() { return effect_; }

    void show()
    {
        centerWidgets(this);
        QMainWindow::show();
    }

    void focus()
    {
        inner().searchbox().selectAll();
        inner().searchbox().setFocus();
    }
};
