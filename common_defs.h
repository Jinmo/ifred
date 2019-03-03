#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include <QtGui>

#include <functional>
extern QHash<ushort, QSet<QString>> g_intern;
extern QSet<QString> g_search;

extern bool highlightTable[65536];
extern QHash<QString, QDate> g_last_used;

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

const char kStyleSheet[] = R"(
                    font-family: Segoe UI;
                    background: #fff;

                    QListView {border: none;}

                    QScrollBar:vertical {
                        border: none;
                        background: #f5f5f5;
                        width: 12px;
                        border-radius: 6px;
                    }

                    QScrollBar::handle:vertical {
                        background: #dadada;
                        min-height: 20px;
                        border-radius: 6px;
                    }

                    QScrollBar::add-line:vertical {
                        width: 0; height: 0;
                        subcontrol-position: bottom;
                        subcontrol-origin: margin;
                    }

                    QScrollBar::sub-line:vertical {
                        width: 0; height: 0;
                        subcontrol-position: top;
                        subcontrol-origin: margin;
                    }

                    QFrame {
                        border-radius: 7px;
                    }
                    QLineEdit, QLineEdit:hover, QLineEdit:active {
                        font-size: 27px;
                        width: 720px;
                        height: 63px;
                        border: none;
                    }

                    )";

const char kItemStyleSheet[] = R"(
                    div {
                        font-family: Segoe UI;
                        font-size: 18px;
                        display: block;
                        color: #959DA6;
                    }

                    span {
                        font-weight: bold;
                        color: #F2973D;
                    }
)";

#endif // COMMON_DEFS_H
