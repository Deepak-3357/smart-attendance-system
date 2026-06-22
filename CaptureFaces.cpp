#include "FaceRecognition.h"
#include "Recognition/RecognitionUtils.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

namespace
{
    constexpr int SampleCount = 100;
    constexpr int FaceSize = 200;
    constexpr const char* FacesDir = "faces";
    constexpr const char* CascadePath = "haarcascade_frontalface_default.xml";

    std::string captureInstruction(int captured)
    {
        const int phase = captured / 14;

        switch (phase)
        {
        case 0:
            return "Look straight - front face";
        case 1:
            return "Turn slightly LEFT";
        case 2:
            return "Turn slightly RIGHT";
        case 3:
            return "Look slightly UP";
        case 4:
            return "Look slightly DOWN";
        case 5:
            return "Smile naturally";
        default:
            return "Neutral face";
        }
    }
}

bool captureFaceDataset(const StudentManager& studentManager)
{
    std::string regNo;
    std::cout << "Enter Register Number: ";
    std::getline(std::cin, regNo);
    regNo = StudentManager::trim(regNo);

    Student student;
    if (!studentManager.getStudentByRegNo(regNo, student))
    {
        std::cout << "Student not found. Register the student before collecting a face dataset.\n";
        return false;
    }

    cv::CascadeClassifier faceDetector;
    if (!faceDetector.load(CascadePath))
    {
        std::cout << "Unable to load Haar Cascade file: " << CascadePath << '\n';
        return false;
    }

    const int cameraIndex = RecognitionUtils::promptCameraIndex();
    cv::VideoCapture camera(cameraIndex);
    if (!camera.isOpened())
    {
        std::cout << "Unable to open webcam. Check camera permission or device availability.\n";
        return false;
    }

    std::filesystem::create_directories(FacesDir);

    int captured = 0;
    cv::Mat frame;

    std::cout << "Collecting 100 multi-angle face samples for " << student.name << ". Press ESC to cancel.\n";

    while (captured < SampleCount)
    {
        camera >> frame;
        if (frame.empty())
        {
            std::cout << "Camera frame is empty. Retrying...\n";
            continue;
        }

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        std::vector<cv::Rect> faces;
        faceDetector.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(80, 80));

        for (const cv::Rect& face : faces)
        {
            if (face.width < 90 || face.height < 90)
            {
                cv::rectangle(frame, face, cv::Scalar(0, 0, 255), 2);
                cv::putText(frame,
                            "Rejected: Face Too Small",
                            cv::Point(20, 70),
                            cv::FONT_HERSHEY_SIMPLEX,
                            0.7,
                            cv::Scalar(0, 0, 255),
                            2);
                continue;
            }

            cv::Mat cropped = gray(face);
            cv::Mat resized;
            cv::resize(cropped, resized, cv::Size(FaceSize, FaceSize));

            const RecognitionUtils::FaceQualityResult quality =
                RecognitionUtils::checkFaceQuality(resized);

            if (!quality.accepted)
            {
                cv::rectangle(frame, face, cv::Scalar(0, 0, 255), 2);
                cv::putText(frame,
                            "Rejected: " + quality.reason,
                            cv::Point(20, 70),
                            cv::FONT_HERSHEY_SIMPLEX,
                            0.7,
                            cv::Scalar(0, 0, 255),
                            2);
                continue;
            }

            ++captured;
            const std::filesystem::path outputPath =
                std::filesystem::path(FacesDir) / (student.regNo + "." + std::to_string(captured) + ".jpg");

            cv::imwrite(outputPath.string(), resized);
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            cv::putText(frame,
                        "Captured " + std::to_string(captured) + " / " + std::to_string(SampleCount),
                        cv::Point(20, 40),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.8,
                        cv::Scalar(0, 255, 0),
                        2);
            cv::putText(frame,
                        captureInstruction(captured),
                        cv::Point(20, 75),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.75,
                        cv::Scalar(255, 255, 0),
                        2);

            std::cout << "Captured " << captured << " / " << SampleCount << '\r';

            if (captured >= SampleCount)
            {
                break;
            }
        }

        if (faces.empty())
        {
            cv::putText(frame,
                        captureInstruction(captured),
                        cv::Point(20, 40),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.75,
                        cv::Scalar(255, 255, 0),
                        2);
            cv::putText(frame,
                        "Keep your face visible inside the camera frame",
                        cv::Point(20, 75),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.65,
                        cv::Scalar(0, 255, 255),
                        2);
        }

        cv::imshow("Capture Face Dataset", frame);
        if (cv::waitKey(30) == 27)
        {
            std::cout << "\nCapture cancelled by user.\n";
            break;
        }
    }

    camera.release();
    cv::destroyWindow("Capture Face Dataset");

    std::cout << "\nDataset capture completed. Samples saved: " << captured << '\n';
    return captured == SampleCount;
}
