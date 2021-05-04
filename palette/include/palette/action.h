#ifndef ACTION_H
#define ACTION_H
#include <QString>
#include <QObject>

struct Action {
    QString id, name, shortcut;
    QString description;
};

Q_DECLARE_METATYPE(Action);

#endif