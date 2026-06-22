#include "DashboardService.h"
#include "../Services/AttendanceInsights.h"

#include <iomanip>
#include <iostream>

DashboardService::DashboardService(const StudentManager& studentManager, const AttendanceManager& attendanceManager)
    : studentManager_(studentManager),
      attendanceManager_(attendanceManager)
{
}

void DashboardService::showRealtimeDashboard() const
{
    const int totalStudents = static_cast<int>(studentManager_.getAllStudents().size());
    const int presentToday = attendanceManager_.countPresentToday();
    const int absentToday = totalStudents > presentToday ? totalStudents - presentToday : 0;
    const double percentage = totalStudents == 0 ? 0.0 : (static_cast<double>(presentToday) / totalStudents) * 100.0;
    const int defaulters = AttendanceInsights::defaulterCount(studentManager_, attendanceManager_);
    const AttendanceInsights::StudentPerformance top =
        AttendanceInsights::topPerformer(studentManager_, attendanceManager_);
    const AttendanceInsights::StudentPerformance lowest =
        AttendanceInsights::lowestPerformer(studentManager_, attendanceManager_);

    std::cout << "\n+---------------------------------------------+\n";
    std::cout << "| SMART ATTENDANCE LIVE DASHBOARD             |\n";
    std::cout << "+---------------------------------------------+\n";
    std::cout << "| Total Students   : " << std::setw(25) << totalStudents << " |\n";
    std::cout << "| Present Today    : " << std::setw(25) << presentToday << " |\n";
    std::cout << "| Absent Today     : " << std::setw(25) << absentToday << " |\n";
    std::cout << "| Attendance %     : " << std::setw(22) << std::fixed << std::setprecision(2) << percentage << "% |\n";
    std::cout << "| Defaulters Count : " << std::setw(25) << defaulters << " |\n";
    std::cout << "+---------------------------------------------+\n";

    if (top.found)
    {
        std::cout << "Top Performer    : " << top.student.name
                  << " (" << std::fixed << std::setprecision(2) << top.percentage << "%)\n";
    }

    if (lowest.found)
    {
        std::cout << "Needs Attention  : " << lowest.student.name
                  << " (" << std::fixed << std::setprecision(2) << lowest.percentage << "%)\n";
    }
}
