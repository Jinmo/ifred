#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QFileSystemWatcher>

extern bool static_updated;
const QString pluginPath(const char *filename);

QJsonObject config();
QString loadFile(const char *filename, bool &updated = static_updated);

class ConfigObserver : public QFileSystemWatcher
{
  public:
    ConfigObserver(QWidget *parent, const char *filename = "styles.json")
        : QFileSystemWatcher(parent)
    {
        addPath(pluginPath(filename));
    }

    void updated()
    {
        onConfigUpdated(config());
    }

    void activate()
    {
        connect(this, &QFileSystemWatcher::fileChanged, this, &ConfigObserver::updated);
        updated();
    }

    virtual void onConfigUpdated(QJsonObject &config) = 0;
};
