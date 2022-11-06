#pragma once

#include "data.hpp"

class Classifier
{
public:
    explicit Classifier(std::vector<Digit> dataSet, int k);
    void classify(const std::vector<cv::Point2f>& path);
    int getResult() const;
    std::vector<cv::Point2f> getSimplifiedPath() const;
private:
    const std::vector<Digit> c_dataSet;
    const int c_simplifiedSize;
    const int c_k;
    int m_result = -1;
    std::vector<cv::Point2f> m_simplifiedPath;
    void simplify(std::vector<cv::Point2f> path);
};
