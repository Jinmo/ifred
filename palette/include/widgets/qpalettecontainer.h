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

		void onUpdated(QJsonObject& config) override {
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
	QPaletteContainer()
		: inner_stacked_(new QStackedWidget(this)), shadow_observer_(new ShadowObserver(this)) {
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

		shadow_observer_->activate();
		setCentralWidget(inner_stacked_);
	}

	void clear() {
		while (inner_stacked_->count())
			inner_stacked_->removeWidget(inner_stacked_->widget(0));
	}

	void add(QPaletteInner * delegate) {
		inner_stacked_->addWidget(delegate);
	}

	void show() {
		centerWidgets(this);
		QMainWindow::show();
	}

	void showWidget(int index = 0) {
		activateWindow();

		if (inner_stacked_->count() <= index)
			return;

		auto * inner = static_cast<QPaletteInner*>(inner_stacked_->widget(index));

		inner->searchbox().selectAll();
		inner->searchbox().setFocus();
	}
};
