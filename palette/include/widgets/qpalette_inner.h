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

class PALETTE_EXPORT QPaletteInner : public QFrame {
protected:
	QItems* entries_;
	QVBoxLayout* layout_;
	QSearch* searchbox_;

	CSSObserver* css_observer_;
	class StylesObserver : public JSONObserver {
	public:
		StylesObserver(QPaletteInner* parent) : JSONObserver(parent, "theme/styles.json") {}

		void onUpdated(QJsonObject& data) override {
			auto width = data["width"].toInt();
			auto parentWidget = static_cast<QPaletteInner *>(parent());

			if (!width) width = 750;
			parentWidget->resize(width, 0);
		}

	} *styles_observer_;

public:
	QSearch& searchbox() { return *searchbox_; }

	QItems& entries() { return *entries_; }

	QPaletteInner(QWidget* parent, const QVector<Action> &items);

	void processEnterResult(EnterResult res);

	void onTextChanged(const QString&) {
		entries_->setCurrentIndex(entries_->model()->index(0, 0));
		entries_->scrollToTop();
		entries_->repaint();
	}

	void onEnterPressed() {
		auto action = entries().model()->data(entries().model()->index(entries_->currentIndex().row(), 0)).value<Action>();
		auto res = enter_callback(action);
		processEnterResult(res);
	}

	bool onArrowPressed(int key) {
		int delta;
		if (key == Qt::Key_Down) {
			delta = 1;
		}
		else {
			delta = -1;
		}
		auto new_row = entries_->currentIndex().row() + delta;
		if (new_row == -1)
			new_row = 0;
		else if (new_row == entries_->model()->rowCount())
			new_row = 0;
		entries_->setCurrentIndex(entries_->model()->index(new_row, 0));
		return true;
	}

	virtual EnterResult enter_callback(Action &action) = 0;

	bool eventFilter(QObject * obj, QEvent * event) override {
		switch (event->type()) {
		case QEvent::KeyPress: {
			auto* ke = static_cast<QKeyEvent*>(event);
			switch (ke->key()) {
			case Qt::Key_Down:
			case Qt::Key_Up: {
				event->ignore();
				return onArrowPressed(ke->key());
			}
			case Qt::Key_Enter:
			case Qt::Key_Return: {
				event->ignore();
				onEnterPressed();
				return true;
			}
			case Qt::Key_Escape: {
				closeWindow();
			}
			default:
				return QFrame::eventFilter(obj, event);
			}
		}
		case QEvent::ShortcutOverride: {
			event->accept();
			return true;
		}
		default:
			return QFrame::eventFilter(obj, event);
		}
	}

	void keyPressEvent(QKeyEvent * e) override {
		if (e->key() != Qt::Key_Escape)
			QFrame::keyPressEvent(e);
		else {
			closeWindow();
		}
	}

	void closeWindow() {
		if (window())
			window()->close();
	}
};
