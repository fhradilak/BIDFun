#pragma once

#include <opencv2/opencv.hpp>

class TouchVisualizer
{
public:
    static void draw(cv::Mat & frame, const cv::RotatedRect & position, cv::Mat & output);
};