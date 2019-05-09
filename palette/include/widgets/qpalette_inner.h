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
	QHash<QKeySequence, QShortcut *> registered_keys_;

    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

	void arrowPressed(int delta);

	template<typename T>
	QShortcut *registerShortcut(QKeySequence sequence, T callback, bool override=true) {
        // QKeyEvent treats Meta key as Ctrl, but QAction doesn't.
        QKeySequence keySequence(sequence.toString().replace("Meta+", "Ctrl+"));
        
        auto *shortcut = new QShortcut(keySequence, this);
		connect(shortcut, &QShortcut::activated, callback);

        if(override)
            registered_keys_.insert(sequence, shortcut);
        
		return shortcut;
	}

public:
    QPaletteInner(QWidget* parent, const QString& name, const QVector<Action>& items, const QString& closeKey);

    void setPlaceholderText(const QString& placeholder);

signals:
    bool itemClicked(Action& action);
    void closed();
};
