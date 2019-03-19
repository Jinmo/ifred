#pragma once

#include <QWidget>
#include <QRect>

#include <widgets/qpalette_inner.h>
#include <utils.h>

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

    void set_inner(QPaletteInner * delegate);

    void show(bool focus);

    void activate();
};
