#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

class DigitVisualizer
{
public:
    void draw(const cv::Mat& colorFrame, std::vector<cv::Point2f> path, cv::Mat& outputFrame) const;
    void setSimplifiedPath(const std::vector<cv::Point2f>& simplifiedPath);
    void setResult(int result);
private:
    std::vector<cv::Point2f> m_simplifiedPath;
    int m_result = -1;
};
