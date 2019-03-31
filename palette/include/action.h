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

    //Action(const Action& other) : id_(other.id_), description_(other.description_), shortcut_(other.shortcut_) {}
    //Action(Action&& other) : id_(std::move(other.id_)), description_(std::move(other.description_)), shortcut_(std::move(other.shortcut_)) {}
    //Action& operator=(Action&& other) {
    //    return Action(other);
    //}

    void swap(Action& rhs) {
        id_.swap(rhs.id_);
        description_.swap(rhs.description_);
        shortcut_.swap(rhs.shortcut_);
    }

protected:
    QString id_, description_, shortcut_;
};

Q_DECLARE_METATYPE(Action);

