#include "EmailService.h"
#include "../Recognition/RecognitionUtils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

EmailService::EmailService(const std::string& configPath)
    : configPath_(configPath)
{
    ensureConfig();
}

bool EmailService::sendReportDraft(const std::string& reportPath) const
{
    const std::string recipient = configuredRecipient();
    if (recipient.empty())
    {
        std::cout << "Configure recipient email in " << configPath_ << " first.\n";
        return false;
    }

    std::ifstream report(reportPath);
    if (!report)
    {
        std::cout << "Report not found: " << reportPath << '\n';
        return false;
    }

    std::filesystem::create_directories("outbox");
    const std::filesystem::path draftPath =
        std::filesystem::path("outbox") / ("attendance_report_" + RecognitionUtils::timestampForFile() + ".eml");

    std::ofstream draft(draftPath.string(), std::ios::trunc);
    draft << "To: " << recipient << '\n';
    draft << "Subject: Smart Attendance Report\n";
    draft << "Content-Type: text/plain; charset=utf-8\n\n";
    draft << "Smart Attendance report generated at " << RecognitionUtils::timestampForDisplay() << "\n\n";
    draft << "CSV report content:\n\n";
    draft << report.rdbuf();

    std::cout << "Email draft generated: " << draftPath.string() << '\n';
    std::cout << "Attach/send this draft with your configured mail client or SMTP gateway.\n";
    return true;
}

std::string EmailService::configuredRecipient() const
{
    std::ifstream input(configPath_);
    std::string header;
    std::string recipient;
    std::getline(input, header);
    std::getline(input, recipient);
    return recipient == "change-me@example.com" ? "" : recipient;
}

void EmailService::ensureConfig() const
{
    std::ifstream input(configPath_);
    if (input.good() && input.peek() != std::ifstream::traits_type::eof())
    {
        return;
    }

    std::ofstream output(configPath_, std::ios::trunc);
    output << "RecipientEmail\n";
    output << "change-me@example.com\n";
}
