#pragma once

#include <string>

class AdminAuth
{
public:
    explicit AdminAuth(const std::string& credentialPath = "admin_credentials.dat");

    bool login() const;
    bool authenticate(const std::string& username, const std::string& password) const;

private:
    std::string credentialPath_;

    void ensureCredentialFile() const;
    bool verify(const std::string& username, const std::string& password) const;
    static std::string hashCredential(const std::string& username, const std::string& password);
};
