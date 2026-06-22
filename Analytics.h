#pragma once

#include "Attendance.h"
#include "Student.h"

class Analytics
{
public:
    Analytics(const StudentManager& studentManager, const AttendanceManager& attendanceManager);

    void showSummary() const;
    void showPerStudentPercentage() const;
    void showDefaulterAnalytics() const;
    void showAttendanceTrend() const;

private:
    const StudentManager& studentManager_;
    const AttendanceManager& attendanceManager_;
};
