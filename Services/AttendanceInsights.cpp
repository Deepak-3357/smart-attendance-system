#include "AttendanceInsights.h"

double AttendanceInsights::attendancePercentage(const AttendanceManager& attendanceManager, const std::string& regNo)
{
    const int workingDays = attendanceManager.countWorkingDays();
    if (workingDays == 0)
    {
        return 0.0;
    }

    return (static_cast<double>(attendanceManager.countPresentDays(regNo)) / workingDays) * 100.0;
}

std::string AttendanceInsights::classification(double percentage)
{
    if (percentage >= 90.0)
    {
        return "EXCELLENT";
    }
    if (percentage >= 75.0)
    {
        return "GOOD";
    }
    if (percentage >= 60.0)
    {
        return "WARNING";
    }

    return "DEFAULTER";
}

int AttendanceInsights::defaulterCount(const StudentManager& studentManager, const AttendanceManager& attendanceManager)
{
    int count = 0;
    for (const Student& student : studentManager.getAllStudents())
    {
        if (attendancePercentage(attendanceManager, student.regNo) < 75.0)
        {
            ++count;
        }
    }

    return count;
}

AttendanceInsights::StudentPerformance AttendanceInsights::topPerformer(
    const StudentManager& studentManager,
    const AttendanceManager& attendanceManager)
{
    StudentPerformance best;

    for (const Student& student : studentManager.getAllStudents())
    {
        const double percentage = attendancePercentage(attendanceManager, student.regNo);
        if (!best.found || percentage > best.percentage)
        {
            best.student = student;
            best.percentage = percentage;
            best.found = true;
        }
    }

    return best;
}

AttendanceInsights::StudentPerformance AttendanceInsights::lowestPerformer(
    const StudentManager& studentManager,
    const AttendanceManager& attendanceManager)
{
    StudentPerformance lowest;

    for (const Student& student : studentManager.getAllStudents())
    {
        const double percentage = attendancePercentage(attendanceManager, student.regNo);
        if (!lowest.found || percentage < lowest.percentage)
        {
            lowest.student = student;
            lowest.percentage = percentage;
            lowest.found = true;
        }
    }

    return lowest;
}
