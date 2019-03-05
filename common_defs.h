#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>
#include <functional>
#include <QtWidgets>

extern QHash<ushort, QSet<QString>> g_intern;
extern QSet<QString> g_search;

extern char highlightTable[65536];
extern QHash<QString, QDate> g_last_used;

class FredCallback
{
  public:
    typedef const std::function<void(int)> handler_t;
    enum
    {
        ARROW,
        ENTER
    };

    handler_t *arrow_callback_, *enter_callback_;

    void setCallback(int type, handler_t &callback)
    {
        switch (type)
        {
        case ARROW:
            arrow_callback_ = &callback;
            break;
        case ENTER:
            enter_callback_ = &callback;
            break;
        }
    }
};

#define WIDTH 755
#define HEIGHT 440

#define CSSLOADER(filename)                        \
    bool updated;                                  \
    auto &buf = loadFile(filename, updated);       \
    setStyleSheet(buf);

#include "utils.h"

QJsonObject config();

#endif // COMMON_DEFS_H
