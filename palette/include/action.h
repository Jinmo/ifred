#pragma once
#include <QString>
#include <QObject>

class Action {
public:
    Action() {}
    ~Action() {}

    Action(const QString& id, const QString& description, const QString& shortcut)
        : id_(id),
        description_(description),
        shortcut_(shortcut) {}

    const QString &id() const { return id_; }
    const QString &description() { return description_; }
    const QString &shortcut() { return shortcut_; }

    void swap(Action& rhs) {
        id_.swap(rhs.id_);
        description_.swap(rhs.description_);
        shortcut_.swap(rhs.shortcut_);
    }

protected:
    QString id_, description_, shortcut_;
};

Q_DECLARE_METATYPE(Action);

