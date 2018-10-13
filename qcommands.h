#ifndef QCOMMANDS_H
#define QCOMMANDS_H

#include <QtGui>
#include <QtWidgets>
#include "common_defs.h"

class QCommands : public QListView, public FredCallback
{
public:
    QCommands();
};

#endif // QCOMMANDS_H
