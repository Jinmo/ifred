#pragma once

#include <QtGui>
#include <QtWidgets>

#include <widgets/qitems.h>
#include <action.h>

#include "palette_export.h"

class QItems;

class PALETTE_EXPORT QPaletteInner : public QFrame {
    Q_OBJECT;

    QString name_;

    QLineEdit* searchbox_;
    QItems* items_;
    QShortcut* shortcut_;

	// Registered shortcuts not overriden by Qt::ShortcutOverride event
	QSet<QKeySequence> registered_keys_;

    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

	void arrowPressed(int delta);

	template<typename T>
	QShortcut *registerShortcut(QKeySequence sequence, T callback, bool override=true) {
		if(override)
			registered_keys_.insert(sequence);

		auto *shortcut = new QShortcut(QKeySequence(sequence), this);
		connect(shortcut, &QShortcut::activated, callback);

		return shortcut;
	}

public:
    QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items, const QString& closeKey);

    void setPlaceholderText(const QString& placeholder);

signals:
    bool itemClicked(Action& action);
    void closed();
};
