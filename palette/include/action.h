#pragma once
#include <QString>
#include <QObject>

struct Action {
    QString id, description, shortcut;
};

Q_DECLARE_METATYPE(Action);

