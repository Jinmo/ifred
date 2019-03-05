#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>
#include <functional>
#include <QtWidgets>

extern char highlightTable[65536];
extern QHash<QString, QDate> g_last_used;

#define WIDTH 755
#define HEIGHT 440

#define CSSLOADER(filename)                        \
    bool updated;                                  \
    auto &buf = loadFile(filename, updated);       \
    setStyleSheet(buf);

#include "utils.h"

QJsonObject config();

#endif // COMMON_DEFS_H
