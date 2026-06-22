#include "RecognitionUtils.h"
#include "../Services/AuditLogger.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace
{
    double g_confidenceThreshold = 45.0;
    constexpr size_t MaxUnknownImages = 500;

    std::tm localTime()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm timeInfo{};
        localtime_s(&timeInfo, &nowTime);
        return timeInfo;
    }
}

int RecognitionUtils::confidenceToAccuracy(double confidence)
{
    const double accuracy = 100.0 - (confidence * 0.12);
    return std::clamp(static_cast<int>(std::round(accuracy)), 0, 100);
}

double RecognitionUtils::getConfidenceThreshold()
{
    return g_confidenceThreshold;
}

void RecognitionUtils::setConfidenceThreshold(double threshold)
{
    g_confidenceThreshold = std::clamp(threshold, 20.0, 90.0);
}

double RecognitionUtils::promptConfidenceThreshold()
{
    std::cout << "Current LBPH confidence threshold: " << g_confidenceThreshold << '\n';
    std::cout << "Enter new threshold between 20 and 90, or press ENTER to keep current: ";

    std::string input;
    std::getline(std::cin, input);

    if (!input.empty())
    {
        try
        {
            setConfidenceThreshold(std::stod(input));
        }
        catch (...)
        {
            std::cout << "Invalid threshold. Keeping " << g_confidenceThreshold << ".\n";
        }
    }

    return g_confidenceThreshold;
}

int RecognitionUtils::promptCameraIndex()
{
    std::cout << "Select Camera [0/1/2]: ";
    std::string input;
    std::getline(std::cin, input);

    try
    {
        const int cameraIndex = std::stoi(input);
        if (cameraIndex >= 0 && cameraIndex <= 2)
        {
            return cameraIndex;
        }
    }
    catch (...)
    {
    }

    std::cout << "Invalid camera selection. Using Camera 0.\n";
    return 0;
}

RecognitionUtils::FaceQualityResult RecognitionUtils::checkFaceQuality(const cv::Mat& grayFace)
{
    if (grayFace.empty())
    {
        return { false, "Empty face crop" };
    }

    cv::Mat laplacian;
    cv::Laplacian(grayFace, laplacian, CV_64F);

    cv::Scalar mean;
    cv::Scalar stdDev;
    cv::meanStdDev(laplacian, mean, stdDev);
    const double blurScore = stdDev.val[0] * stdDev.val[0];
    const double brightness = cv::mean(grayFace).val[0];

    if (blurScore < 65.0)
    {
        return { false, "Face too blurry" };
    }

    if (brightness < 45.0)
    {
        return { false, "Face too dark" };
    }

    if (brightness > 235.0)
    {
        return { false, "Face too bright" };
    }

    return { true, "Good quality" };
}

std::string RecognitionUtils::timestampForFile()
{
    const std::tm timeInfo = localTime();
    std::ostringstream output;
    output << std::put_time(&timeInfo, "%Y_%m_%d_%H_%M_%S");
    return output.str();
}

std::string RecognitionUtils::timestampForDisplay()
{
    const std::tm timeInfo = localTime();
    std::ostringstream output;
    output << std::put_time(&timeInfo, "%d-%m-%Y %H:%M:%S");
    return output.str();
}

bool RecognitionUtils::saveUnknownFace(const cv::Mat& grayFace)
{
    if (grayFace.empty())
    {
        return false;
    }

    std::filesystem::create_directories("unknown");
    const std::filesystem::path unknownDir("unknown");
    std::vector<std::filesystem::directory_entry> images;

    for (const auto& entry : std::filesystem::directory_iterator(unknownDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".jpg")
        {
            images.push_back(entry);
        }
    }

    std::sort(images.begin(), images.end(), [](const auto& left, const auto& right) {
        return std::filesystem::last_write_time(left.path()) <
               std::filesystem::last_write_time(right.path());
    });

    while (images.size() >= MaxUnknownImages)
    {
        std::error_code ignored;
        std::filesystem::remove(images.front().path(), ignored);
        images.erase(images.begin());
    }

    const std::string timestamp = timestampForFile();
    const std::filesystem::path outputPath =
        unknownDir / ("unknown_" + timestamp + ".jpg");

    const bool saved = cv::imwrite(outputPath.string(), grayFace);
    if (saved)
    {
        std::ofstream log(unknownDir / "unknown_log.csv", std::ios::app);
        log << outputPath.filename().string() << ','
            << timestamp.substr(0, 10) << ','
            << timestamp.substr(11) << '\n';

        AuditLogger::log("Unknown Face Detected", "Image: " + outputPath.string());
    }

    return saved;
}
