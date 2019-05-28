#pragma once
#include <QString>
#include <QObject>

struct Action {
    QString id, name, shortcut;
    QString description;
};

Q_DECLARE_METATYPE(Action);

