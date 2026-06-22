#pragma once

#include <QString>
#include <QStringList>
#include <optional>
#include <vector>

struct UserRecord {
    QString fullName;
    QString username;
    QString passwordHash;
    QString role;
};

class UserManager {
public:
    explicit UserManager(QString usersFilePath = QString());

    QString usersFilePath() const { return usersFilePath_; }
    std::vector<UserRecord> allUsers() const;
    std::optional<UserRecord> findUser(const QString& username) const;

    bool createUser(const QString& fullName,
                    const QString& username,
                    const QString& password,
                    const QString& role,
                    QString* errorMessage = nullptr);

    bool updateUser(const QString& username,
                    const QString& fullName,
                    const QString& role,
                    QString* errorMessage = nullptr);

    bool deleteUser(const QString& username, QString* errorMessage = nullptr);
    bool resetPassword(const QString& username, const QString& newPassword, QString* errorMessage = nullptr);
    bool verifyPassword(const QString& username, const QString& password, UserRecord* matchedUser = nullptr) const;

    static QString hashPassword(const QString& password);
    static QStringList availableRoles();

private:
    QString usersFilePath_;

    void ensureInitialized();
    std::vector<UserRecord> loadUsers() const;
    bool saveUsers(const std::vector<UserRecord>& users, QString* errorMessage = nullptr) const;
    bool usernameExists(const std::vector<UserRecord>& users, const QString& username) const;
    int administratorCount(const std::vector<UserRecord>& users) const;

    static UserRecord defaultAdmin();
    static QString normalizedUsername(const QString& username);
    static QString normalizedRole(const QString& role);
    static QString escapeCsv(const QString& value);
    static QStringList parseCsvLine(const QString& line);
};
