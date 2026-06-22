#pragma once

#include <opencv2/core.hpp>

#include <string>

namespace RecognitionUtils
{
    struct FaceQualityResult
    {
        bool accepted;
        std::string reason;
    };

    int confidenceToAccuracy(double confidence);
    double getConfidenceThreshold();
    void setConfidenceThreshold(double threshold);
    double promptConfidenceThreshold();
    int promptCameraIndex();
    FaceQualityResult checkFaceQuality(const cv::Mat& grayFace);
    std::string timestampForFile();
    std::string timestampForDisplay();
    bool saveUnknownFace(const cv::Mat& grayFace);
}
