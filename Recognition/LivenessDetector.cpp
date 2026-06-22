#include "LivenessDetector.h"

#include <opencv2/imgproc.hpp>

#include <vector>

namespace
{
    constexpr int RequiredConsecutiveEyeFrames = 10;
    constexpr int ProxyTimeoutFrames = 180;
}

LivenessDetector::LivenessDetector(const std::string& eyeCascadePath)
{
    available_ = eyeDetector_.load(eyeCascadePath);
}

bool LivenessDetector::isAvailable() const
{
    return available_;
}

bool LivenessDetector::update(int label, const cv::Mat& grayFace)
{
    if (!available_ || grayFace.empty())
    {
        return true;
    }

    State& state = states_[label];
    const bool openNow = eyesOpen(grayFace);
    ++state.framesWaiting;

    if (openNow)
    {
        ++state.consecutiveEyeFrames;
        ++state.totalEyeFrames;
        state.consecutiveNoEyeFrames = 0;
    }
    else
    {
        state.consecutiveEyeFrames = 0;
        ++state.consecutiveNoEyeFrames;
    }

    if (state.consecutiveEyeFrames >= RequiredConsecutiveEyeFrames)
    {
        state.verified = true;
        state.proxyDetected = false;
    }
    else if (!state.verified &&
             state.framesWaiting >= ProxyTimeoutFrames &&
             state.totalEyeFrames == 0)
    {
        state.proxyDetected = true;
    }

    return state.verified;
}

std::string LivenessDetector::statusText(int label) const
{
    if (!available_)
    {
        return "LIVE VERIFIED";
    }

    const auto found = states_.find(label);
    if (found == states_.end())
    {
        return "Blink to verify";
    }

    if (found->second.verified)
    {
        return "LIVE VERIFIED";
    }

    if (found->second.proxyDetected)
    {
        return "PROXY DETECTED";
    }

    return "Verifying eyes " +
        std::to_string(found->second.consecutiveEyeFrames) +
        " / " +
        std::to_string(RequiredConsecutiveEyeFrames);
}

bool LivenessDetector::eyesOpen(const cv::Mat& grayFace)
{
    cv::Mat upperFace = grayFace(cv::Rect(0, 0, grayFace.cols, grayFace.rows / 2));
    std::vector<cv::Rect> eyes;

    eyeDetector_.detectMultiScale(upperFace, eyes, 1.1, 3, 0, cv::Size(18, 18));
    return eyes.size() >= 2;
}
