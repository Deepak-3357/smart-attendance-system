#include "AdminAuth.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace
{
    constexpr const char* DefaultUser = "admin";
    constexpr const char* DefaultPassword = "admin123";
    constexpr const char* Salt = "SmartAttendanceSystem.Admin.v1";
}

AdminAuth::AdminAuth(const std::string& credentialPath)
    : credentialPath_(credentialPath)
{
    ensureCredentialFile();
}

bool AdminAuth::login() const
{
    for (int attempt = 1; attempt <= 3; ++attempt)
    {
        std::string username;
        std::string password;

        std::cout << "\n=================================\n";
        std::cout << "ADMIN LOGIN\n";
        std::cout << "=================================\n";
        std::cout << "Username: ";
        std::getline(std::cin, username);
        std::cout << "Password: ";
        std::getline(std::cin, password);

        if (verify(username, password))
        {
            std::cout << "Login successful.\n";
            return true;
        }

        std::cout << "Invalid credentials. Attempts left: " << (3 - attempt) << '\n';
    }

    return false;
}

bool AdminAuth::authenticate(const std::string& username, const std::string& password) const
{
    return verify(username, password);
}

void AdminAuth::ensureCredentialFile() const
{
    std::ifstream input(credentialPath_);
    if (input.good() && input.peek() != std::ifstream::traits_type::eof())
    {
        return;
    }

    std::ofstream output(credentialPath_, std::ios::trunc);
    output << DefaultUser << ',' << hashCredential(DefaultUser, DefaultPassword) << '\n';
}

bool AdminAuth::verify(const std::string& username, const std::string& password) const
{
    std::ifstream input(credentialPath_);
    std::string savedUser;
    std::string savedHash;

    if (std::getline(input, savedUser, ',') && std::getline(input, savedHash))
    {
        return username == savedUser && hashCredential(username, password) == savedHash;
    }

    return false;
}

std::string AdminAuth::hashCredential(const std::string& username, const std::string& password)
{
    const std::string input = std::string(Salt) + ":" + username + ":" + password;
    unsigned long long hash = 1469598103934665603ULL;

    for (unsigned char ch : input)
    {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }

    std::ostringstream output;
    output << std::hex << std::setw(16) << std::setfill('0') << hash;
    return output.str();
}
