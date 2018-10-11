#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>
extern QHash<ushort, QSet<QString>> g_intern;
extern QSet<QString> g_search;

extern bool highlightTable[65536];

#endif // COMMON_DEFS_H
