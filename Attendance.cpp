#include "Attendance.h"
#include "CsvUtils.h"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>

namespace
{
    constexpr const char* AttendanceHeader = "Date,Time,RegNo,Name,Status";

    std::tm localTime()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm timeInfo{};
        localtime_s(&timeInfo, &nowTime);
        return timeInfo;
    }
}

AttendanceManager::AttendanceManager(const std::string& attendancePath, const std::string& reportsDir)
    : attendancePath_(attendancePath),
      reportsDir_(reportsDir)
{
    ensureStorage();
}

bool AttendanceManager::markPresent(const Student& student)
{
    ensureStorage();

    if (student.regNo.empty() || student.name.empty())
    {
        std::cout << "Invalid student data. Attendance not marked.\n";
        return false;
    }

    if (isMarkedToday(student.regNo))
    {
        std::cout << "Attendance already marked today for " << student.name << " (" << student.regNo << ").\n";
        return false;
    }

    AttendanceRecord record{
        currentDate(),
        currentTime(),
        student.regNo,
        student.name,
        "Present"
    };

    appendRecord(record, attendancePath_);

    const std::filesystem::path dailyPath = std::filesystem::path(reportsDir_) / (record.date + ".csv");
    CsvUtils::ensureHeader(dailyPath.string(), AttendanceHeader);
    appendRecord(record, dailyPath.string());

    std::cout << "Attendance marked for " << student.name << " (" << student.regNo << ").\n";
    return true;
}

bool AttendanceManager::isMarkedToday(const std::string& regNo) const
{
    const std::string today = currentDate();
    for (const AttendanceRecord& record : getAllRecords())
    {
        if (record.date == today && record.regNo == regNo && record.status == "Present")
        {
            return true;
        }
    }

    return false;
}

std::vector<AttendanceRecord> AttendanceManager::getAllRecords() const
{
    ensureStorage();

    std::ifstream input(attendancePath_);
    if (!input)
    {
        throw std::runtime_error("Unable to open attendance file: " + attendancePath_);
    }

    std::vector<AttendanceRecord> records;
    std::string line;
    bool firstLine = true;

    while (std::getline(input, line))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }

        if (CsvUtils::trim(line).empty())
        {
            continue;
        }

        const std::vector<std::string> fields = CsvUtils::parseLine(line);
        if (fields.size() >= 5)
        {
            records.push_back({ fields[0], fields[1], fields[2], fields[3], fields[4] });
        }
    }

    return records;
}

std::vector<AttendanceRecord> AttendanceManager::getRecordsForDate(const std::string& date) const
{
    std::vector<AttendanceRecord> result;

    for (const AttendanceRecord& record : getAllRecords())
    {
        if (record.date == date)
        {
            result.push_back(record);
        }
    }

    return result;
}

int AttendanceManager::countPresentToday() const
{
    std::set<std::string> students;

    for (const AttendanceRecord& record : getRecordsForDate(currentDate()))
    {
        if (record.status == "Present")
        {
            students.insert(record.regNo);
        }
    }

    return static_cast<int>(students.size());
}

int AttendanceManager::countPresentDays(const std::string& regNo) const
{
    std::set<std::string> days;

    for (const AttendanceRecord& record : getAllRecords())
    {
        if (record.regNo == regNo && record.status == "Present")
        {
            days.insert(record.date);
        }
    }

    return static_cast<int>(days.size());
}

int AttendanceManager::countAbsentDays(const std::string& regNo) const
{
    const int workingDays = countWorkingDays();
    const int presentDays = countPresentDays(regNo);
    return workingDays > presentDays ? workingDays - presentDays : 0;
}

int AttendanceManager::countWorkingDays() const
{
    std::set<std::string> days;

    for (const AttendanceRecord& record : getAllRecords())
    {
        if (record.status == "Present")
        {
            days.insert(record.date);
        }
    }

    return static_cast<int>(days.size());
}

void AttendanceManager::viewAttendance() const
{
    const std::vector<AttendanceRecord> records = getAllRecords();

    std::cout << "\n=================================\n";
    std::cout << "Attendance Records\n";
    std::cout << "=================================\n";

    if (records.empty())
    {
        std::cout << "No attendance records found.\n";
        return;
    }

    std::cout << std::left
              << std::setw(14) << "Date"
              << std::setw(10) << "Time"
              << std::setw(16) << "Reg No"
              << std::setw(24) << "Name"
              << "Status\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (const AttendanceRecord& record : records)
    {
        std::cout << std::left
                  << std::setw(14) << record.date
                  << std::setw(10) << record.time
                  << std::setw(16) << record.regNo
                  << std::setw(24) << record.name
                  << record.status << '\n';
    }
}

std::string AttendanceManager::currentDate()
{
    const std::tm timeInfo = localTime();
    std::ostringstream output;
    output << std::put_time(&timeInfo, "%d-%m-%Y");
    return output.str();
}

std::string AttendanceManager::currentTime()
{
    const std::tm timeInfo = localTime();
    std::ostringstream output;
    output << std::put_time(&timeInfo, "%H:%M");
    return output.str();
}

void AttendanceManager::ensureStorage() const
{
    std::filesystem::create_directories(reportsDir_);
    CsvUtils::ensureHeader(attendancePath_, AttendanceHeader);
}

void AttendanceManager::appendRecord(const AttendanceRecord& record, const std::string& path) const
{
    std::ofstream output(path, std::ios::app);
    if (!output)
    {
        throw std::runtime_error("Unable to write attendance file: " + path);
    }

    output << CsvUtils::escape(record.date) << ','
           << CsvUtils::escape(record.time) << ','
           << CsvUtils::escape(record.regNo) << ','
           << CsvUtils::escape(record.name) << ','
           << CsvUtils::escape(record.status) << '\n';
}
