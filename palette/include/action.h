#pragma once
#include <QString>
#include <QObject>

class Action {
    //Q_OBJECT
public:
    Action() {}
    Action(const Action& other) : id_(other.id_), description_(other.description_), shortcut_(other.shortcut_) {
    }
    ~Action() {}

    Action(const QString& id, const QString& description, const QString& shortcut)
        : id_(id),
        description_(description),
        shortcut_(shortcut) {}

    const QString& id() const { return id_; }
    const QString& description() const { return description_; }
    const QString& shortcut() const { return shortcut_; }

protected:
    const QString id_, description_, shortcut_;
};

Q_DECLARE_METATYPE(Action);

