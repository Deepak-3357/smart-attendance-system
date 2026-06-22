#pragma once

#include "../Attendance.h"
#include "../Student.h"

class DashboardService
{
public:
    DashboardService(const StudentManager& studentManager, const AttendanceManager& attendanceManager);

    void showRealtimeDashboard() const;

private:
    const StudentManager& studentManager_;
    const AttendanceManager& attendanceManager_;
};
