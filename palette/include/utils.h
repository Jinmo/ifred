#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QFileSystemWatcher>

static bool static_updated;

const QJsonObject json(const char* filename, bool force_update = false);

QString loadFile(const char* filename, bool force_update = false, bool& updated = static_updated);

// File handler
typedef const QString(*pathhandler_t)(char const* path);
extern pathhandler_t pluginPath;

class JSONObserver : public QFileSystemWatcher {
private:
    const char* filename_;
public:
    JSONObserver(QWidget* parent, const char* filename = "settings.json")
        : QFileSystemWatcher(parent), filename_(filename) {
        addPath(pluginPath(filename));
    }

    void updated() {
        onUpdated(json(filename_, true));
    }

    void activate() {
        connect(this, &QFileSystemWatcher::fileChanged, this, &JSONObserver::updated);
        updated();
    }

    virtual void onUpdated(const QJsonObject& config) = 0;
};
