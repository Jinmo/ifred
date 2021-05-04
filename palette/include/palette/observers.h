#pragma once

#include <QWidget>
#include <QFileSystemWatcher>

#include <palette/utils.h>

class CSSObserver : public QFileSystemWatcher {
    const char* filename_;
public:
    CSSObserver(QWidget* parent, const char* filename) : QFileSystemWatcher(parent), filename_(filename) {
        addPath(pluginPath(filename));
        connect(this, &QFileSystemWatcher::fileChanged, this, &CSSObserver::updated);

        updated();
    }

    void updated() {
        parentWidget()->setStyleSheet(loadFile(filename_));
    }

    QWidget* parentWidget() {
        return static_cast<QWidget*>(parent());
    }
};

class JSONObserver : public QFileSystemWatcher {
    const char* filename_;
protected:
    JSONObserver(QObject* parent, const char* filename = "settings.json")
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
