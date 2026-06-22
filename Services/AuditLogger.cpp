#include "AuditLogger.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

void AuditLogger::log(const std::string& eventName, const std::string& details)
{
    std::filesystem::create_directories("logs");

    std::ofstream output("logs/system.log", std::ios::app);
    if (!output)
    {
        return;
    }

    output << '[' << timestamp() << "]\n";
    output << eventName << '\n';
    if (!details.empty())
    {
        output << details << '\n';
    }
    output << '\n';
}

std::string AuditLogger::timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm timeInfo{};
    localtime_s(&timeInfo, &nowTime);

    std::ostringstream output;
    output << std::put_time(&timeInfo, "%d-%m-%Y %H:%M");
    return output.str();
}
