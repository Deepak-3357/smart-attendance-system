#include "Analytics.h"
#include "Attendance.h"
#include "Dashboard/DashboardService.h"
#include "FaceRecognition.h"
#include "Recognition/RecognitionUtils.h"
#include "Reports/ReportService.h"
#include "Services/AdminAuth.h"
#include "Services/AuditLogger.h"
#include "Services/ConsoleUI.h"
#include "Services/EmailService.h"
#include "Services/ModelStatisticsService.h"
#include "Services/ProfileService.h"
#include "Student.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <limits>

namespace
{
    void ensureProjectFolders()
    {
        std::filesystem::create_directories("faces");
        std::filesystem::create_directories("trainer");
        std::filesystem::create_directories("attendance");
        std::filesystem::create_directories("reports");
        std::filesystem::create_directories("unknown");
        std::filesystem::create_directories("outbox");
        std::filesystem::create_directories("logs");
    }

    void pause()
    {
        std::cout << "\nPress ENTER to continue...";
        std::string ignored;
        std::getline(std::cin, ignored);
    }

    int readChoice()
    {
        std::cout << "\nEnter choice: ";
        std::string input;
        std::getline(std::cin, input);

        try
        {
            return std::stoi(input);
        }
        catch (...)
        {
            return -1;
        }
    }

    bool hasTrainingImages()
    {
        if (!std::filesystem::exists("faces"))
        {
            return false;
        }

        for (const auto& entry : std::filesystem::directory_iterator("faces"))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".jpg")
            {
                return true;
            }
        }

        return false;
    }

    bool registerStudent(StudentManager& studentManager)
    {
        std::string regNo;
        std::string name;

        std::cout << "Enter Register Number: ";
        std::getline(std::cin, regNo);

        std::cout << "Enter Student Name: ";
        std::getline(std::cin, name);

        const bool registered = studentManager.registerStudent(regNo, name);
        if (registered)
        {
            AuditLogger::log(
                "Student Registration",
                "RegNo:" + StudentManager::trim(regNo) + "\nName:" + StudentManager::trim(name));
        }

        return registered;
    }

    void searchStudents(const StudentManager& studentManager)
    {
        std::string query;
        std::cout << "Search by Reg No or Name: ";
        std::getline(std::cin, query);

        const std::vector<Student> results = studentManager.search(query);
        if (results.empty())
        {
            std::cout << "No students matched your search.\n";
            return;
        }

        for (const Student& student : results)
        {
            std::cout << student.regNo << " - " << student.name << '\n';
        }
    }

    void showMenu()
    {
        ConsoleUI::printHeader("SMART ATTENDANCE SYSTEM");
        std::cout << "1. Register Student\n";
        std::cout << "2. Capture Face Dataset\n";
        std::cout << "3. Train Model\n";
        std::cout << "4. Recognize Face\n";
        std::cout << "5. View Students\n";
        std::cout << "6. View Attendance\n";
        std::cout << "7. Analytics\n";
        std::cout << "8. Search Student\n";
        std::cout << "9. View Date Wise Reports\n";
        std::cout << "10. Export Attendance Report\n";
        std::cout << "11. Real-Time Dashboard\n";
        std::cout << "12. View Student Profile\n";
        std::cout << "13. Email Attendance Report\n";
        std::cout << "14. Tune Recognition Threshold\n";
        std::cout << "15. Model Statistics\n";
        std::cout << "16. Generate Project Statistics\n";
        std::cout << "17. Exit\n";
    }
}

int main()
{
    try
    {
        ensureProjectFolders();
        ConsoleUI::printStartupBanner();

        AdminAuth adminAuth;
        if (!adminAuth.login())
        {
            ConsoleUI::printError("Access denied.");
            return 1;
        }
        AuditLogger::log("Admin Login", "Status: Success");

        StudentManager studentManager("students.csv");
        AttendanceManager attendanceManager("attendance.csv", "reports");
        Analytics analytics(studentManager, attendanceManager);
        ReportService reportService(studentManager, attendanceManager);
        DashboardService dashboardService(studentManager, attendanceManager);
        ProfileService profileService(studentManager, attendanceManager);
        ModelStatisticsService modelStatisticsService(studentManager, attendanceManager);
        EmailService emailService;
        modelStatisticsService.generateProjectStatistics();

        bool running = true;
        while (running)
        {
            showMenu();

            switch (readChoice())
            {
            case 1:
                if (registerStudent(studentManager) && hasTrainingImages())
                {
                    ConsoleUI::printWarning("Student list changed. Auto retraining existing model...");
                    trainModel();
                }
                pause();
                break;
            case 2:
                if (captureFaceDataset(studentManager))
                {
                    AuditLogger::log("Dataset Capture", "Status: Completed");
                    ConsoleUI::printWarning("Dataset updated. Auto retraining model...");
                    trainModel();
                }
                pause();
                break;
            case 3:
                trainModel();
                pause();
                break;
            case 4:
                recognizeFaces(studentManager, attendanceManager);
                pause();
                break;
            case 5:
                studentManager.viewStudents();
                pause();
                break;
            case 6:
                attendanceManager.viewAttendance();
                pause();
                break;
            case 7:
                analytics.showSummary();
                analytics.showPerStudentPercentage();
                analytics.showDefaulterAnalytics();
                analytics.showAttendanceTrend();
                pause();
                break;
            case 8:
                searchStudents(studentManager);
                pause();
                break;
            case 9:
                reportService.viewDateWiseReports();
                pause();
                break;
            case 10:
                reportService.exportAttendanceReport();
                pause();
                break;
            case 11:
                dashboardService.showRealtimeDashboard();
                pause();
                break;
            case 12:
                profileService.viewStudentProfile();
                pause();
                break;
            case 13:
                if (reportService.exportAttendanceReport())
                {
                    emailService.sendReportDraft("attendance_report.csv");
                }
                pause();
                break;
            case 14:
                RecognitionUtils::promptConfidenceThreshold();
                pause();
                break;
            case 15:
                modelStatisticsService.showModelStatistics();
                pause();
                break;
            case 16:
                if (modelStatisticsService.generateProjectStatistics())
                {
                    ConsoleUI::printSuccess("Generated PROJECT_STATISTICS.txt");
                    AuditLogger::log("Project Statistics Generated", "File: PROJECT_STATISTICS.txt");
                }
                else
                {
                    ConsoleUI::printError("Unable to generate PROJECT_STATISTICS.txt");
                }
                pause();
                break;
            case 17:
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                pause();
                break;
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
