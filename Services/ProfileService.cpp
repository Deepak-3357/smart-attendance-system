#include "ProfileService.h"
#include "AttendanceInsights.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <filesystem>
#include <iomanip>
#include <iostream>

ProfileService::ProfileService(const StudentManager& studentManager, const AttendanceManager& attendanceManager)
    : studentManager_(studentManager),
      attendanceManager_(attendanceManager)
{
}

void ProfileService::viewStudentProfile() const
{
    std::string regNo;
    std::cout << "Enter Register Number: ";
    std::getline(std::cin, regNo);
    regNo = StudentManager::trim(regNo);

    Student student;
    if (!studentManager_.getStudentByRegNo(regNo, student))
    {
        std::cout << "Student not found.\n";
        return;
    }

    std::filesystem::path latestImage;
    std::filesystem::file_time_type latestTime{};
    bool foundImage = false;

    if (std::filesystem::exists("faces"))
    {
        for (const auto& entry : std::filesystem::directory_iterator("faces"))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const std::string filename = entry.path().filename().string();
            if (filename.rfind(student.regNo + ".", 0) == 0)
            {
                const auto modified = std::filesystem::last_write_time(entry.path());
                if (!foundImage || modified > latestTime)
                {
                    latestImage = entry.path();
                    latestTime = modified;
                    foundImage = true;
                }
            }
        }
    }

    std::cout << "\n=================================\n";
    std::cout << "Student Profile\n";
    std::cout << "=================================\n";
    const int presentDays = attendanceManager_.countPresentDays(student.regNo);
    const int absentDays = attendanceManager_.countAbsentDays(student.regNo);
    const double percentage = AttendanceInsights::attendancePercentage(attendanceManager_, student.regNo);

    std::cout << "Name           : " << student.name << '\n';
    std::cout << "RegNo          : " << student.regNo << '\n';
    std::cout << "Present Days   : " << presentDays << '\n';
    std::cout << "Absent Days    : " << absentDays << '\n';
    std::cout << "Attendance %   : " << std::fixed << std::setprecision(2) << percentage << "%\n";
    std::cout << "Classification : " << AttendanceInsights::classification(percentage) << '\n';

    if (!foundImage)
    {
        std::cout << "No dataset image found.\n";
        return;
    }

    std::cout << "Image : " << latestImage.string() << '\n';

    cv::Mat image = cv::imread(latestImage.string());
    if (image.empty())
    {
        std::cout << "Unable to preview dataset image.\n";
        return;
    }

    cv::imshow("Student Profile - " + student.regNo, image);
    std::cout << "Press any key in the image window to close preview.\n";
    cv::waitKey(0);
    cv::destroyWindow("Student Profile - " + student.regNo);
}
