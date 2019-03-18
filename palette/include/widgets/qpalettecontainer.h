#pragma once

#include <QWidget>
#include <QRect>

#include <widgets/qpalette_inner.h>
#include <utils.h>

static void centerWidgets(QWidget* widget, QWidget* host = nullptr) {
    if (!host)
        host = widget->parentWidget();

    if (host) {
        auto hostRect = host->geometry();
        widget->move(hostRect.center() - widget->rect().center());
    }
    else {
        QRect screenGeometry = QApplication::desktop()->screenGeometry();
        int x = (screenGeometry.width() - widget->width()) / 2;
        int y = (screenGeometry.height() - widget->height()) / 2;
        widget->move(x, y);
    }
}

class QPaletteContainer : public QMainWindow {
    QStackedWidget* inner_stacked_;

    class ShadowObserver : public JSONObserver {
    public:
        ShadowObserver(QPaletteContainer* parent) : JSONObserver(parent, "theme/styles.json") {}

        void onUpdated(const QJsonObject& config) override {
            int kShadow = config["shadow"].toInt();

            if (kShadow == 0) {
                qDebug() << "config not valid: shadow == 0";
                kShadow = 20;
            }

            auto* effect = new QGraphicsDropShadowEffect();

            effect->setBlurRadius(kShadow);
            effect->setColor(QColor(0, 0, 0, 100));
            effect->setOffset(0);

            QPaletteContainer* owner = static_cast<QPaletteContainer*>(parent());
            owner->setGraphicsEffect(effect);
            owner->setContentsMargins(kShadow, kShadow, kShadow, kShadow);
        }

    } *shadow_observer_;

public:
    QPaletteContainer();

    void set_inner(QPaletteInner * delegate) {
        while (inner_stacked_->count())
            inner_stacked_->removeWidget(inner_stacked_->widget(0));
        inner_stacked_->addWidget(delegate);
    }

    void show(bool focus) {
        centerWidgets(this);
        QMainWindow::show();

        if(focus)
            activate();
    }

    void activate() {
        activateWindow();

        if (!inner_stacked_->count())
            return;

        auto * inner = static_cast<QPaletteInner*>(inner_stacked_->widget(0));

        inner->searchbox().selectAll();
        inner->searchbox().setFocus();
    }
};
