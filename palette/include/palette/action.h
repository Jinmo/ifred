#ifndef ACTION_H
#define ACTION_H
#include <QObject>
#include <QString>

struct Action {
  QString id, name, shortcut;
  QString description;
};

Q_DECLARE_METATYPE(Action);

#endif