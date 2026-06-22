#include "Analytics.h"
#include "Services/AttendanceInsights.h"
#include "Services/ConsoleUI.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>

Analytics::Analytics(const StudentManager& studentManager, const AttendanceManager& attendanceManager)
    : studentManager_(studentManager),
      attendanceManager_(attendanceManager)
{
}

void Analytics::showSummary() const
{
    ConsoleUI::showLoading("Generating analytics");

    const int totalStudents = static_cast<int>(studentManager_.getAllStudents().size());
    const int presentToday = attendanceManager_.countPresentToday();
    const int absentToday = totalStudents > presentToday ? totalStudents - presentToday : 0;

    std::cout << "\n=================================\n";
    std::cout << "Attendance Analytics\n";
    std::cout << "=================================\n";
    std::cout << "Total Students : " << totalStudents << '\n';
    std::cout << "Present Today  : " << presentToday << '\n';
    std::cout << "Absent Today   : " << absentToday << '\n';
    std::cout << "Defaulters     : "
              << AttendanceInsights::defaulterCount(studentManager_, attendanceManager_) << '\n';
}

void Analytics::showPerStudentPercentage() const
{
    const std::vector<Student> students = studentManager_.getAllStudents();
    const int workingDays = attendanceManager_.countWorkingDays();

    std::cout << "\n=================================\n";
    std::cout << "Per Student Attendance Percentage\n";
    std::cout << "=================================\n";

    if (students.empty())
    {
        std::cout << "No students registered.\n";
        return;
    }

    if (workingDays == 0)
    {
        std::cout << "No working days found in attendance records.\n";
        return;
    }

    std::cout << std::left
              << std::setw(16) << "Reg No"
              << std::setw(24) << "Name"
              << std::setw(14) << "Present Days"
              << std::setw(12) << "Absent"
              << std::setw(16) << "Attendance %"
              << "Status\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (const Student& student : students)
    {
        const int presentDays = attendanceManager_.countPresentDays(student.regNo);
        const int absentDays = attendanceManager_.countAbsentDays(student.regNo);
        const double percentage = AttendanceInsights::attendancePercentage(attendanceManager_, student.regNo);

        std::cout << std::left
                  << std::setw(16) << student.regNo
                  << std::setw(24) << student.name
                  << std::setw(14) << presentDays
                  << std::setw(12) << absentDays
                  << std::setw(16) << (std::to_string(static_cast<int>(percentage)) + "%")
                  << AttendanceInsights::classification(percentage) << '\n';
    }
}

void Analytics::showDefaulterAnalytics() const
{
    std::cout << "\n=================================\n";
    std::cout << "Defaulter Analytics\n";
    std::cout << "=================================\n";
    std::cout << std::left
              << std::setw(16) << "Reg No"
              << std::setw(24) << "Student Name"
              << std::setw(16) << "Attendance %"
              << "Status\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (const Student& student : studentManager_.getAllStudents())
    {
        const double percentage = AttendanceInsights::attendancePercentage(attendanceManager_, student.regNo);
        const std::string status = percentage < 75.0 ? "DEFAULTER" : "GOOD";

        std::cout << std::left
                  << std::setw(16) << student.regNo
                  << std::setw(24) << student.name
                  << std::setw(16) << (std::to_string(static_cast<int>(percentage)) + "%")
                  << status << '\n';
    }
}

void Analytics::showAttendanceTrend() const
{
    std::map<std::string, std::set<std::string>> presentByDate;

    for (const AttendanceRecord& record : attendanceManager_.getAllRecords())
    {
        if (record.status == "Present")
        {
            presentByDate[record.date].insert(record.regNo);
        }
    }

    std::filesystem::create_directories("reports");
    std::ofstream output("reports/attendance_trend.csv", std::ios::trunc);
    output << "Date,Present Count\n";

    std::cout << "\n=================================\n";
    std::cout << "Attendance Trend Analytics\n";
    std::cout << "=================================\n";
    std::cout << std::left << std::setw(16) << "Date" << "Present Count\n";
    std::cout << "---------------------------------\n";

    for (const auto& item : presentByDate)
    {
        std::cout << std::left << std::setw(16) << item.first << item.second.size() << '\n';
        output << item.first << ',' << item.second.size() << '\n';
    }

    std::cout << "Trend report generated: reports/attendance_trend.csv\n";
}
