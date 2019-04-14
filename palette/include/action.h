#pragma once
#include <QString>
#include <QObject>

class Action {
public:
    void swap(Action& rhs) {
        id_.swap(rhs.id_);
        description_.swap(rhs.description_);
        shortcut_.swap(rhs.shortcut_);
    }


    Action() {}
    ~Action() {}

    Action(const Action& other) : id_(other.id_), description_(other.description_), shortcut_(other.shortcut_) {
    }

    Action(Action&& other) = default;

    Action(const QString& id, const QString& description, const QString& shortcut)
        : id_(id),
        description_(description),
        shortcut_(shortcut) {
    }

    const QString& id() const { return id_; }
    const QString& description() { return description_; }
    const QString& shortcut() { return shortcut_; }

    Action& operator=(Action&& other) noexcept {
        other.swap(*this);
        return *this;
    }

    Action& operator=(const Action& other) noexcept {
        id_ = other.id_;
        description_ = other.description_;
        shortcut_ = other.shortcut_;

        return *this;
    }

protected:
    QString id_, description_, shortcut_;
};

template<>
void std::swap(Action& lhs, Action& rhs) noexcept;

Q_DECLARE_METATYPE(Action);

