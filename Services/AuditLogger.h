#pragma once

#include <string>

class AuditLogger
{
public:
    static void log(const std::string& eventName, const std::string& details = "");

private:
    static std::string timestamp();
};
