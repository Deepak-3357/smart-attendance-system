#pragma once

#include "Student.h"

#include <string>
#include <vector>

struct AttendanceRecord
{
    std::string date;
    std::string time;
    std::string regNo;
    std::string name;
    std::string status;
};

class AttendanceManager
{
public:
    explicit AttendanceManager(const std::string& attendancePath = "attendance.csv",
                               const std::string& reportsDir = "reports");

    bool markPresent(const Student& student);
    bool isMarkedToday(const std::string& regNo) const;
    std::vector<AttendanceRecord> getAllRecords() const;
    std::vector<AttendanceRecord> getRecordsForDate(const std::string& date) const;
    int countPresentToday() const;
    int countPresentDays(const std::string& regNo) const;
    int countAbsentDays(const std::string& regNo) const;
    int countWorkingDays() const;
    void viewAttendance() const;

    static std::string currentDate();
    static std::string currentTime();

private:
    std::string attendancePath_;
    std::string reportsDir_;

    void ensureStorage() const;
    void appendRecord(const AttendanceRecord& record, const std::string& path) const;
};
