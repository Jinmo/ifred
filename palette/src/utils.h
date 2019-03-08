#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QFileSystemWatcher>

extern bool static_updated;

const QString pluginPath(const char *filename);

QJsonObject config(const char *filename, bool force_update=false);

QString loadFile(const char *filename, bool force_update=false, bool &updated = static_updated);

class ConfigObserver : public QFileSystemWatcher {
private:
	const char* filename_;
public:
    ConfigObserver(QWidget *parent, const char *filename = "settings.json")
            : QFileSystemWatcher(parent), filename_(filename) {
        addPath(pluginPath(filename));
    }

    void updated() {
        onConfigUpdated(config(filename_, true));
    }

    void activate() {
        connect(this, &QFileSystemWatcher::fileChanged, this, &ConfigObserver::updated);
        updated();
    }

    virtual void onConfigUpdated(QJsonObject &config) = 0;
};
