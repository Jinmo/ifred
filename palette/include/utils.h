#pragma once

#include <QtGui>
#include <QtWidgets>

#include <QFileSystemWatcher>
#include <functional>

static bool static_updated;

const QJsonObject json(const char* filename, bool force_update = false);

QString loadFile(const char* filename, bool force_update = false, bool& updated = static_updated);

// File handler
typedef const QString(*pathhandler_t)(char const* path);
extern pathhandler_t pluginPath;