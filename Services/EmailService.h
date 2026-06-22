#pragma once

#include <string>

class EmailService
{
public:
    explicit EmailService(const std::string& configPath = "email_config.csv");

    bool sendReportDraft(const std::string& reportPath) const;

private:
    std::string configPath_;

    std::string configuredRecipient() const;
    void ensureConfig() const;
};
