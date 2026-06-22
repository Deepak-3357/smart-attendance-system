#include "AuthenticationService.h"
#include "../Auth/UserManager.h"

AuthenticationService::AuthenticationService(UserManager* userManager)
    : userManager_(userManager) {}

bool AuthenticationService::login(const QString& username,
                                  const QString& password,
                                  CurrentUser* authenticatedUser,
                                  QString* errorMessage) const {
    UserRecord matchedUser;
    if (!userManager_ || !userManager_->verifyPassword(username, password, &matchedUser)) {
        if (errorMessage) {
            *errorMessage = "Invalid Username or Password";
        }
        return false;
    }

    const CurrentUser user{matchedUser.fullName, matchedUser.username, matchedUser.role};
    if (authenticatedUser) {
        *authenticatedUser = user;
    }
    CurrentUser::setCurrent(user);
    return true;
}
