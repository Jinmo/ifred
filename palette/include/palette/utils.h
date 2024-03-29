#pragma once

#include <QFileSystemWatcher>
#include <QtGui>
#include <QtWidgets>
#include <functional>

static bool static_updated;

QJsonObject json(const char* filename, bool force_update = false);

QString loadFile(const char* filename, bool force_update = false,
                 bool& updated = static_updated);

// File handler
typedef QString (*pathhandler_t)(char const* path);
extern pathhandler_t pluginPath;