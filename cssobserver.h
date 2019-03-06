#pragma once

#include <QWidget>
#include <QFileSystemWatcher>

#include "utils.h"

class CSSObserver : public QFileSystemWatcher
{
    const char *filename_;
  public:
    CSSObserver(QWidget *parent, const char *filename) : QFileSystemWatcher(parent), filename_(filename)
    {
        addPath(pluginPath(filename));
        connect(this, &QFileSystemWatcher::fileChanged, this, &CSSObserver::updated);

        updated();
    }

    void updated() {
        parentWidget()->setStyleSheet(loadFile(filename_));
    }

    QWidget *parentWidget() {
        return static_cast<QWidget *>(parent());
    }
};