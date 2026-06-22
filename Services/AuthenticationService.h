#pragma once

#include "../Auth/CurrentUser.h"

#include <QString>

class UserManager;

class AuthenticationService {
public:
    explicit AuthenticationService(UserManager* userManager);

    bool login(const QString& username,
               const QString& password,
               CurrentUser* authenticatedUser,
               QString* errorMessage = nullptr) const;

private:
    UserManager* userManager_;
};
