#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class Digit
{
public:
    Digit(std::vector<cv::Point2i> points, int label);
    const std::vector<cv::Point2i> & points() const;
    int label() const;
private:
    std::vector<cv::Point2i> m_points;
    int m_label;
};

class DataSet
{
public:
    static std::vector<Digit> read(const std::string& fileName, int numberOfRows);
    static std::vector<Digit> read(const std::string& fileName);
};

class DataVisualizer
{
public:
    static void showNumber(std::vector<Digit> dataSet, int number, int scale = 2);
};