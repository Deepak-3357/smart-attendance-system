#pragma once

#include <QString>

class CurrentUser {
public:
    QString fullName;
    QString username;
    QString role;

    bool isValid() const { return !username.trimmed().isEmpty(); }
    bool isAdministrator() const { return role == "Administrator"; }
    bool isFaculty() const { return role == "Faculty"; }
    bool isViewer() const { return role == "Viewer"; }

    static void setCurrent(const CurrentUser& user) {
        storage() = user;
        hasStorage() = user.isValid();
    }

    static CurrentUser current() { return storage(); }
    static bool hasCurrent() { return hasStorage(); }

    static void clearCurrent() {
        storage() = CurrentUser{};
        hasStorage() = false;
    }

private:
    static CurrentUser& storage() {
        static CurrentUser user;
        return user;
    }

    static bool& hasStorage() {
        static bool hasUser = false;
        return hasUser;
    }
};
