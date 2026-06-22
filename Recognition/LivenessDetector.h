#pragma once

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

#include <map>
#include <string>

class LivenessDetector
{
public:
    explicit LivenessDetector(const std::string& eyeCascadePath = "haarcascade_eye.xml");

    bool isAvailable() const;
    bool update(int label, const cv::Mat& grayFace);
    std::string statusText(int label) const;

private:
    struct State
    {
        bool closedSeen = false;
        bool verified = false;
        bool proxyDetected = false;
        int framesWaiting = 0;
        int consecutiveEyeFrames = 0;
        int totalEyeFrames = 0;
        int consecutiveNoEyeFrames = 0;
    };

    cv::CascadeClassifier eyeDetector_;
    bool available_ = false;
    std::map<int, State> states_;

    bool eyesOpen(const cv::Mat& grayFace);
};
