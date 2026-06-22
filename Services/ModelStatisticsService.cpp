#include "ModelStatisticsService.h"
#include "AttendanceInsights.h"
#include "../Recognition/RecognitionUtils.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

ModelStatisticsService::ModelStatisticsService(
    const StudentManager& studentManager,
    const AttendanceManager& attendanceManager)
    : studentManager_(studentManager),
      attendanceManager_(attendanceManager)
{
}

void ModelStatisticsService::showModelStatistics() const
{
    const int totalStudents = static_cast<int>(studentManager_.getAllStudents().size());
    const int totalFaceImages = countImagesInFolder("faces");
    const int unknownImages = countImagesInFolder("unknown");
    const unsigned long long trainerSize = fileSizeBytes("trainer/trainer.yml");
    std::map<std::string, int> imagesPerStudent;

    if (std::filesystem::exists("faces"))
    {
        for (const auto& entry : std::filesystem::directory_iterator("faces"))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".jpg")
            {
                continue;
            }

            const std::string filename = entry.path().filename().string();
            const size_t dot = filename.find('.');
            if (dot != std::string::npos)
            {
                ++imagesPerStudent[filename.substr(0, dot)];
            }
        }
    }

    std::cout << "\n=================================\n";
    std::cout << "Model Statistics\n";
    std::cout << "=================================\n";
    std::cout << "Students          : " << totalStudents << '\n';
    std::cout << "Face Images       : " << totalFaceImages << '\n';
    std::cout << "Threshold         : " << RecognitionUtils::getConfidenceThreshold() << '\n';
    std::cout << "Trainer Size      : " << std::fixed << std::setprecision(2)
              << (static_cast<double>(trainerSize) / (1024.0 * 1024.0)) << " MB\n";
    std::cout << "Unknown Captures  : " << unknownImages << '\n';
    std::cout << "Images Per Student:\n";

    for (const Student& student : studentManager_.getAllStudents())
    {
        std::cout << "  " << student.regNo << " - " << student.name << " : "
                  << imagesPerStudent[student.regNo] << '\n';
    }
}

bool ModelStatisticsService::generateProjectStatistics(const std::string& path) const
{
    std::ofstream output(path, std::ios::trunc);
    if (!output)
    {
        return false;
    }

    output << "SMART ATTENDANCE MANAGEMENT SYSTEM\n";
    output << "PROJECT STATISTICS\n\n";
    output << "Total Students           : " << studentManager_.getAllStudents().size() << '\n';
    output << "Total Images             : " << countImagesInFolder("faces") << '\n';
    output << "Total Attendance Records : " << attendanceManager_.getAllRecords().size() << '\n';
    output << "Model Type               : LBPH Face Recognizer\n";
    output << "Threshold                : " << RecognitionUtils::getConfidenceThreshold() << '\n';
    output << "Defaulters Count         : "
           << AttendanceInsights::defaulterCount(studentManager_, attendanceManager_) << '\n';

    return true;
}

int ModelStatisticsService::countImagesInFolder(const std::string& folder)
{
    if (!std::filesystem::exists(folder))
    {
        return 0;
    }

    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".jpg")
        {
            ++count;
        }
    }

    return count;
}

unsigned long long ModelStatisticsService::fileSizeBytes(const std::string& path)
{
    std::error_code ignored;
    return std::filesystem::exists(path, ignored)
        ? static_cast<unsigned long long>(std::filesystem::file_size(path, ignored))
        : 0ULL;
}
