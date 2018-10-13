#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>

#include <functional>
extern QHash<ushort, QSet<QString>> g_intern;
extern QSet<QString> g_search;

extern bool highlightTable[65536];

class FredCallback {
public:
    typedef const std::function<void (int)> handler_t;
    enum {
        ARROW,
        ENTER
    };

    handler_t *arrow_callback_, *enter_callback_;

    void setCallback(int type, handler_t &callback) {
        switch(type) {
        case ARROW:
            arrow_callback_ = &callback;
            break;
        case ENTER:
            enter_callback_ = &callback;
            break;
        }
    }
};

#endif // COMMON_DEFS_H
