#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qpalette_inner.h"

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
    T inner_;

  public:
    QPaletteInner &inner() { return inner_; }
    QPalette_()
    {
        const int kShadow = 30;

        inner().setParent(this);

        setWindowFlags(Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
        effect->setBlurRadius(kShadow);
        effect->setColor(QColor(0, 0, 0, 100));
        effect->setOffset(0);

        inner().setGraphicsEffect(effect);
        setCentralWidget(&inner());

        setContentsMargins(kShadow, kShadow, kShadow, kShadow);
    }
    void show()
    {
        centerWidgets(this);
        QMainWindow::show();
    }
    void focus()
    {
        inner_.searchbox().selectAll();
        inner_.searchbox().setFocus();
    }
};
