#pragma once

#include "../Attendance.h"
#include "../Student.h"

#include <string>

class ModelStatisticsService
{
public:
    ModelStatisticsService(const StudentManager& studentManager, const AttendanceManager& attendanceManager);

    void showModelStatistics() const;
    bool generateProjectStatistics(const std::string& path = "PROJECT_STATISTICS.txt") const;

private:
    const StudentManager& studentManager_;
    const AttendanceManager& attendanceManager_;

    static int countImagesInFolder(const std::string& folder);
    static unsigned long long fileSizeBytes(const std::string& path);
};
