#include "FaceRecognition.h"
#include "Services/AuditLogger.h"
#include "Services/ConsoleUI.h"

#include <opencv2/face.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace
{
    constexpr const char* FacesDir = "faces";
    constexpr const char* TrainerDir = "trainer";
    constexpr const char* TrainerPath = "trainer/trainer.yml";

    bool parseLabel(const std::filesystem::path& imagePath, int& label)
    {
        const std::string filename = imagePath.filename().string();
        const size_t dotPosition = filename.find('.');
        if (dotPosition == std::string::npos)
        {
            return false;
        }

        try
        {
            label = std::stoi(filename.substr(0, dotPosition));
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}

bool trainModel()
{
    ConsoleUI::showLoading("Training LBPH model");

    if (!std::filesystem::exists(FacesDir))
    {
        std::cout << "Faces folder not found. Capture datasets before training.\n";
        AuditLogger::log("Model Training Failed", "Reason: faces folder not found");
        return false;
    }

    std::vector<cv::Mat> images;
    std::vector<int> labels;

    for (const auto& entry : std::filesystem::directory_iterator(FacesDir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const std::string extension = entry.path().extension().string();
        if (extension != ".jpg" && extension != ".jpeg" && extension != ".png")
        {
            continue;
        }

        int label = 0;
        if (!parseLabel(entry.path(), label))
        {
            std::cout << "Skipping invalid dataset file name: " << entry.path().filename().string() << '\n';
            continue;
        }

        cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
        if (image.empty())
        {
            std::cout << "Skipping unreadable image: " << entry.path().filename().string() << '\n';
            continue;
        }

        cv::resize(image, image, cv::Size(200, 200));
        images.push_back(image);
        labels.push_back(label);
    }

    std::cout << "Images Loaded: " << images.size() << '\n';

    if (images.empty())
    {
        std::cout << "No valid training images found.\n";
        AuditLogger::log("Model Training Failed", "Reason: no valid training images");
        return false;
    }

    std::filesystem::create_directories(TrainerDir);

    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->train(images, labels);
    model->save(TrainerPath);

    std::cout << "Training Completed\n";
    std::cout << "trainer.yml Created Successfully\n";
    AuditLogger::log("Model Training Completed", "Images Loaded: " + std::to_string(images.size()));
    return true;
}
