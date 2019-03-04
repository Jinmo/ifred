#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>
#include <functional>
#include <QtWidgets>

extern QHash<ushort, QSet<QString>> g_intern;
extern QSet<QString> g_search;

extern bool highlightTable[65536];
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

#define WIDTH 720
#define HEIGHT 74

#include <ida.hpp>
#include <idp.hpp>

#include <fpro.h>

#define CSSLOADER(filename)                        \
    QTimer *timer = new QTimer(this);              \
    connect(timer, &QTimer::timeout, this, [=]() { \
        char *buf = CSSLOADER_SYNC(filename);      \
        setStyleSheet(buf);                        \
                                                   \
        delete[] buf;                              \
    });                                            \
    timer->start(1000);

#include <map>

static std::map<std::string, std::pair<long, std::string>> last_loaded;

static char *CSSLOADER_SYNC(const char *filename)
{
    auto absolutePath = (QDir::homePath() + "/take/" + filename).toStdString();
    long timestamp = time(NULL);

    if(last_loaded.count(absolutePath)) {
        std::pair<long, std::string> &csspair = last_loaded[absolutePath];
        if(csspair.first > timestamp - 1) {
            return strdup(csspair.second.c_str());
        }
    }
    FILE *fp = qfopen(absolutePath.c_str(), "r");

    if (fp == NULL)
    {
        char *buf = new char[1];
        buf[0] = 0;
        return buf;
    }

    qfseek(fp, 0, SEEK_END);
    auto size = qftell(fp);
    char *buf = new char[size + 1];
    qfseek(fp, 0, SEEK_SET);
    qfread(fp, buf, size);
    qfclose(fp);

    auto csspair = std::pair<long, std::string>(timestamp, buf);
    last_loaded[absolutePath] = csspair;

    return buf;
}

#endif // COMMON_DEFS_H
