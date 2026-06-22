#pragma once

#include "../Attendance.h"
#include "../Student.h"

#include <string>

namespace AttendanceInsights
{
    struct StudentPerformance
    {
        Student student;
        double percentage = 0.0;
        bool found = false;
    };

    double attendancePercentage(const AttendanceManager& attendanceManager, const std::string& regNo);
    std::string classification(double percentage);
    int defaulterCount(const StudentManager& studentManager, const AttendanceManager& attendanceManager);
    StudentPerformance topPerformer(const StudentManager& studentManager, const AttendanceManager& attendanceManager);
    StudentPerformance lowestPerformer(const StudentManager& studentManager, const AttendanceManager& attendanceManager);
}
