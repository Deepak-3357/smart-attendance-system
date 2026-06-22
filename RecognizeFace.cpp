#include "FaceRecognition.h"
#include "Recognition/LivenessDetector.h"
#include "Recognition/RecognitionUtils.h"
#include "Services/AuditLogger.h"

#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>

namespace
{
    constexpr const char* CascadePath = "haarcascade_frontalface_default.xml";
    constexpr const char* TrainerPath = "trainer/trainer.yml";
    constexpr int RequiredStableFrames = 10;
}

bool recognizeFaces(const StudentManager& studentManager, AttendanceManager& attendanceManager)
{
    if (!std::filesystem::exists(TrainerPath))
    {
        std::cout << "Trainer model not found: " << TrainerPath << ". Train the model first.\n";
        return false;
    }

    cv::CascadeClassifier faceDetector;

    if (!faceDetector.load(CascadePath))
    {
        std::cout << "Unable to load Haar Cascade file: "
            << CascadePath << '\n';
        return false;
    }

    cv::Ptr<cv::face::LBPHFaceRecognizer> model =
        cv::face::LBPHFaceRecognizer::create();

    try
    {
        model->read(TrainerPath);
    }
    catch (const cv::Exception& ex)
    {
        std::cout << "Unable to load trained model: "
            << ex.what() << '\n';
        return false;
    }

    const double knownFaceThreshold = RecognitionUtils::promptConfidenceThreshold();
    const int cameraIndex = RecognitionUtils::promptCameraIndex();

    cv::VideoCapture camera(cameraIndex);

    if (!camera.isOpened())
    {
        std::cout << "Unable to open webcam.\n";
        return false;
    }

    std::cout << "Recognition Started...\n";
    std::cout << "Press ESC to stop.\n";
    AuditLogger::log("Recognition Started", "Camera: " + std::to_string(cameraIndex));

    std::set<std::string> markedThisSession;
    std::map<int, int> stableVotes;
    LivenessDetector livenessDetector;

    std::string popupName;
    std::string popupRegNo;
    std::string popupTime;
    std::chrono::steady_clock::time_point popupUntil{};
    int unknownLogCooldown = 0;

    cv::Mat frame;

    while (true)
    {
        camera >> frame;

        if (frame.empty())
        {
            continue;
        }

        cv::Mat gray;

        cv::cvtColor(
            frame,
            gray,
            cv::COLOR_BGR2GRAY
        );

        cv::equalizeHist(
            gray,
            gray
        );

        std::vector<cv::Rect> faces;

        faceDetector.detectMultiScale(
            gray,
            faces,
            1.1,
            5,
            0,
            cv::Size(80, 80)
        );

        std::set<int> labelsSeenThisFrame;

        for (const cv::Rect& face : faces)
        {
            cv::Mat cropped = gray(face);

            cv::Mat resized;

            cv::resize(
                cropped,
                resized,
                cv::Size(200, 200)
            );

            int predictedLabel = -1;
            double confidence = 0;

            model->predict(
                resized,
                predictedLabel,
                confidence
            );

            std::string line1 =
                "UNKNOWN PERSON";

            std::string line2 =
                "Attendance Not Marked";

            std::string line3 =
                "Accuracy: " +
                std::to_string(RecognitionUtils::confidenceToAccuracy(confidence)) +
                "%";

            cv::Scalar color(
                0,
                0,
                255
            );

            Student student;

            const std::string regNo =
                std::to_string(predictedLabel);

            const bool hasKnownStudent =
                confidence > 0.0 &&
                confidence < knownFaceThreshold &&
                studentManager.getStudentByRegNo(
                    regNo,
                    student
                );

            if (hasKnownStudent)
            {
                const bool firstTimeThisFrame =
                    labelsSeenThisFrame.insert(predictedLabel).second;

                if (firstTimeThisFrame)
                {
                    ++stableVotes[predictedLabel];
                }

                line3 =
                    "VERIFYING... (" +
                    std::to_string(std::min(stableVotes[predictedLabel], RequiredStableFrames)) +
                    "/" +
                    std::to_string(RequiredStableFrames) +
                    ")";
            }

            if (
                hasKnownStudent &&
                stableVotes[predictedLabel] >= RequiredStableFrames)
            {
                line1 =
                    student.name;

                line2 =
                    "RegNo: " +
                    student.regNo +
                    "  Accuracy: " +
                    std::to_string(RecognitionUtils::confidenceToAccuracy(confidence)) +
                    "%";

                color =
                    cv::Scalar(
                        0,
                        255,
                        0
                    );

                const bool liveVerified =
                    livenessDetector.update(predictedLabel, resized);

                if (stableVotes[predictedLabel] > RequiredStableFrames)
                {
                    line3 = livenessDetector.statusText(predictedLabel);
                }

                if (
                    liveVerified &&
                    markedThisSession.find(
                        student.regNo
                    ) ==
                    markedThisSession.end() &&
                    !attendanceManager.isMarkedToday(
                        student.regNo
                    )
                    )
                {
                    if (
                        attendanceManager.markPresent(
                            student
                        )
                        )
                    {
                        markedThisSession.insert(
                            student.regNo
                        );

                        popupName = student.name;
                        popupRegNo = student.regNo;
                        popupTime = AttendanceManager::currentTime();
                        popupUntil = std::chrono::steady_clock::now() + std::chrono::seconds(3);

                        AuditLogger::log(
                            "Attendance Marked",
                            "RegNo:" + student.regNo + "\nName:" + student.name);
                    }
                }
                else if (!liveVerified)
                {
                    color = cv::Scalar(0, 255, 255);
                }
            }
            else if (!hasKnownStudent)
            {
                if (unknownLogCooldown <= 0)
                {
                    RecognitionUtils::saveUnknownFace(resized);
                    unknownLogCooldown = 60;
                }
            }

            cv::rectangle(
                frame,
                face,
                color,
                2
            );

            cv::putText(
                frame,
                line1,
                cv::Point(
                    face.x,
                    std::max(
                        25,
                        face.y - 30
                    )
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.7,
                color,
                2
            );

            cv::putText(
                frame,
                line2,
                cv::Point(
                    face.x,
                    std::max(
                        50,
                        face.y - 8
                    )
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.55,
                color,
                2
            );

            cv::putText(
                frame,
                line3,
                cv::Point(
                    face.x,
                    face.y + face.height + 25
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.55,
                color,
                2
            );
        }

        for (auto& vote : stableVotes)
        {
            if (labelsSeenThisFrame.find(vote.first) == labelsSeenThisFrame.end())
            {
                vote.second = 0;
            }
        }

        if (unknownLogCooldown > 0)
        {
            --unknownLogCooldown;
        }

        if (std::chrono::steady_clock::now() < popupUntil)
        {
            cv::rectangle(frame, cv::Rect(10, 10, 520, 150), cv::Scalar(0, 80, 0), cv::FILLED);
            cv::putText(
                frame,
                "================================",
                cv::Point(
                    20,
                    32
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.55,
                cv::Scalar(
                    255,
                    255,
                    255
                ),
                2
            );

            cv::putText(
                frame,
                "ATTENDANCE MARKED",
                cv::Point(
                    20,
                    62
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.75,
                cv::Scalar(
                    0,
                    255,
                    0
                ),
                3
            );

            cv::putText(
                frame,
                "Name : " + popupName,
                cv::Point(
                    20,
                    92
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.62,
                cv::Scalar(
                    255,
                    255,
                    255
                ),
                2
            );

            cv::putText(
                frame,
                "Reg No : " + popupRegNo,
                cv::Point(
                    20,
                    118
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.62,
                cv::Scalar(
                    255,
                    255,
                    255
                ),
                2
            );

            cv::putText(
                frame,
                "Time : " + popupTime,
                cv::Point(
                    20,
                    144
                ),
                cv::FONT_HERSHEY_SIMPLEX,
                0.62,
                cv::Scalar(
                    255,
                    255,
                    255
                ),
                2
            );
        }

        cv::imshow(
            "Smart Attendance Recognition",
            frame
        );

        if (cv::waitKey(10) == 27)
        {
            break;
        }
    }

    camera.release();

    cv::destroyWindow(
        "Smart Attendance Recognition"
    );

    AuditLogger::log("Recognition Stopped");
    return true;
}
