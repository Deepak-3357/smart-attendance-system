#include "UserManager.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTextStream>

namespace {
QString defaultUsersPath() {
    return QDir("data").filePath("users.csv");
}

void setError(QString* errorMessage, const QString& message) {
    if (errorMessage) {
        *errorMessage = message;
    }
}
}

UserManager::UserManager(QString usersFilePath)
    : usersFilePath_(usersFilePath.trimmed().isEmpty() ? defaultUsersPath() : std::move(usersFilePath)) {
    ensureInitialized();
}

QStringList UserManager::availableRoles() {
    return {"Administrator", "Faculty", "Viewer"};
}

QString UserManager::hashPassword(const QString& password) {
    return QString::fromLatin1(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

std::vector<UserRecord> UserManager::allUsers() const {
    return loadUsers();
}

std::optional<UserRecord> UserManager::findUser(const QString& username) const {
    const QString target = normalizedUsername(username);
    for (const auto& user : loadUsers()) {
        if (normalizedUsername(user.username) == target) {
            return user;
        }
    }
    return std::nullopt;
}

bool UserManager::createUser(const QString& fullName,
                             const QString& username,
                             const QString& password,
                             const QString& role,
                             QString* errorMessage) {
    const QString cleanUsername = normalizedUsername(username);
    const QString cleanRole = normalizedRole(role);

    if (cleanUsername.isEmpty()) {
        setError(errorMessage, "Username cannot be empty.");
        return false;
    }
    if (password.isEmpty()) {
        setError(errorMessage, "Password cannot be empty.");
        return false;
    }
    if (cleanRole.isEmpty()) {
        setError(errorMessage, "Please select a valid role.");
        return false;
    }

    auto users = loadUsers();
    if (usernameExists(users, cleanUsername)) {
        setError(errorMessage, "Username must be unique.");
        return false;
    }

    users.push_back({fullName.trimmed().isEmpty() ? cleanUsername : fullName.trimmed(),
                     cleanUsername,
                     hashPassword(password),
                     cleanRole});
    return saveUsers(users, errorMessage);
}

bool UserManager::updateUser(const QString& username,
                             const QString& fullName,
                             const QString& role,
                             QString* errorMessage) {
    const QString cleanUsername = normalizedUsername(username);
    const QString cleanRole = normalizedRole(role);
    if (cleanUsername.isEmpty()) {
        setError(errorMessage, "No user is selected.");
        return false;
    }
    if (cleanRole.isEmpty()) {
        setError(errorMessage, "Please select a valid role.");
        return false;
    }

    auto users = loadUsers();
    for (auto& user : users) {
        if (normalizedUsername(user.username) == cleanUsername) {
            if (user.role == "Administrator" && cleanRole != "Administrator" && administratorCount(users) <= 1) {
                setError(errorMessage, "At least one administrator account must remain.");
                return false;
            }
            user.fullName = fullName.trimmed().isEmpty() ? user.username : fullName.trimmed();
            user.role = cleanRole;
            return saveUsers(users, errorMessage);
        }
    }

    setError(errorMessage, "Selected user was not found.");
    return false;
}

bool UserManager::deleteUser(const QString& username, QString* errorMessage) {
    const QString cleanUsername = normalizedUsername(username);
    if (cleanUsername.isEmpty()) {
        setError(errorMessage, "No user is selected.");
        return false;
    }

    auto users = loadUsers();
    for (auto it = users.begin(); it != users.end(); ++it) {
        if (normalizedUsername(it->username) == cleanUsername) {
            if (it->role == "Administrator" && administratorCount(users) <= 1) {
                setError(errorMessage, "Cannot delete the last administrator account.");
                return false;
            }
            users.erase(it);
            return saveUsers(users, errorMessage);
        }
    }

    setError(errorMessage, "Selected user was not found.");
    return false;
}

bool UserManager::resetPassword(const QString& username, const QString& newPassword, QString* errorMessage) {
    const QString cleanUsername = normalizedUsername(username);
    if (cleanUsername.isEmpty()) {
        setError(errorMessage, "No user is selected.");
        return false;
    }
    if (newPassword.isEmpty()) {
        setError(errorMessage, "Password cannot be empty.");
        return false;
    }

    auto users = loadUsers();
    for (auto& user : users) {
        if (normalizedUsername(user.username) == cleanUsername) {
            user.passwordHash = hashPassword(newPassword);
            return saveUsers(users, errorMessage);
        }
    }

    setError(errorMessage, "Selected user was not found.");
    return false;
}

bool UserManager::verifyPassword(const QString& username, const QString& password, UserRecord* matchedUser) const {
    const QString cleanUsername = normalizedUsername(username);
    const QString expectedHash = hashPassword(password);
    for (const auto& user : loadUsers()) {
        if (normalizedUsername(user.username) == cleanUsername && user.passwordHash == expectedHash) {
            if (matchedUser) {
                *matchedUser = user;
            }
            return true;
        }
    }
    return false;
}

void UserManager::ensureInitialized() {
    const QFileInfo info(usersFilePath_);
    QDir().mkpath(info.absolutePath());

    QFile file(usersFilePath_);
    if (file.exists() && file.size() > 0 && !loadUsers().empty()) {
        return;
    }

    QString error;
    saveUsers({defaultAdmin()}, &error);
}

std::vector<UserRecord> UserManager::loadUsers() const {
    std::vector<UserRecord> users;
    QFile file(usersFilePath_);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return users;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        const auto fields = parseCsvLine(line);
        if (fields.size() < 4) {
            continue;
        }
        if (fields[0] == "FullName" && fields[1] == "Username") {
            continue;
        }

        users.push_back({fields[0].trimmed(),
                         normalizedUsername(fields[1]),
                         fields[2].trimmed(),
                         normalizedRole(fields[3])});
    }

    return users;
}

bool UserManager::saveUsers(const std::vector<UserRecord>& users, QString* errorMessage) const {
    const QFileInfo info(usersFilePath_);
    QDir().mkpath(info.absolutePath());

    QSaveFile file(usersFilePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(errorMessage, "Unable to open users.csv for writing.");
        return false;
    }

    QTextStream out(&file);
    out << "FullName,Username,PasswordHash,Role\n";
    for (const auto& user : users) {
        out << escapeCsv(user.fullName) << ','
            << escapeCsv(user.username) << ','
            << escapeCsv(user.passwordHash) << ','
            << escapeCsv(user.role) << '\n';
    }

    if (!file.commit()) {
        setError(errorMessage, "Unable to save users.csv.");
        return false;
    }
    return true;
}

bool UserManager::usernameExists(const std::vector<UserRecord>& users, const QString& username) const {
    const QString target = normalizedUsername(username);
    for (const auto& user : users) {
        if (normalizedUsername(user.username) == target) {
            return true;
        }
    }
    return false;
}

int UserManager::administratorCount(const std::vector<UserRecord>& users) const {
    int count = 0;
    for (const auto& user : users) {
        if (user.role == "Administrator") {
            ++count;
        }
    }
    return count;
}

UserRecord UserManager::defaultAdmin() {
    return {"System Administrator", "admin", hashPassword("admin123"), "Administrator"};
}

QString UserManager::normalizedUsername(const QString& username) {
    return username.trimmed().toLower();
}

QString UserManager::normalizedRole(const QString& role) {
    const QString cleanRole = role.trimmed();
    for (const QString& availableRole : availableRoles()) {
        if (availableRole.compare(cleanRole, Qt::CaseInsensitive) == 0) {
            return availableRole;
        }
    }
    return {};
}

QString UserManager::escapeCsv(const QString& value) {
    QString escaped = value;
    escaped.replace('"', "\"\"");
    if (escaped.contains(',') || escaped.contains('"') || escaped.contains('\n')) {
        escaped = '"' + escaped + '"';
    }
    return escaped;
}

QStringList UserManager::parseCsvLine(const QString& line) {
    QStringList fields;
    QString current;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar ch = line.at(i);
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line.at(i + 1) == '"') {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields << current;
            current.clear();
        } else {
            current += ch;
        }
    }
    fields << current;
    return fields;
}
