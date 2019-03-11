#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QFileSystemWatcher>

extern bool static_updated;

const QString pluginPath(const char* filename);

QJsonObject config(const char* filename, bool force_update = false);

QString loadFile(const char* filename, bool force_update = false, bool& updated = static_updated);

class JSONObserver : public QFileSystemWatcher {
private:
	const char* filename_;
public:
	JSONObserver(QWidget* parent, const char* filename = "settings.json")
		: QFileSystemWatcher(parent), filename_(filename) {
		addPath(pluginPath(filename));
	}

	void updated() {
		onUpdated(config(filename_, true));
	}

	void activate() {
		connect(this, &QFileSystemWatcher::fileChanged, this, &JSONObserver::updated);
		updated();
	}

	virtual void onUpdated(QJsonObject& config) = 0;
};
