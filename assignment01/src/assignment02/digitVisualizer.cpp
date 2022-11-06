#include "digitVisualizer.hpp"

#include <string>

void DigitVisualizer::draw(const cv::Mat& colorFrame, std::vector<cv::Point2f> path, cv::Mat& outputFrame) const
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
     * YOU DON'T NEED TO EDIT THIS *
     * BUT YOU'RE ALLOWED TO DO SO *
     *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    colorFrame.copyTo(outputFrame);

    // no input path -> show result
    if(path.empty())
    {
        // no valid result -> don't show anything
        if(m_result < 0) return;

        // show lines and circles
        for (auto i = 0u; i + 1 < m_simplifiedPath.size(); ++i)
        {
            cv::line(outputFrame, m_simplifiedPath[i], m_simplifiedPath[i+1], cv::Scalar(0, 255, 0), 3);
            cv::circle(outputFrame, m_simplifiedPath[i], 9, cv::Scalar(0, 255, 0), 3);
        }
        // last circle
        cv::circle(outputFrame, m_simplifiedPath.back(), 9, cv::Scalar(0, 255, 0), 3);

        // text label
        cv::putText(outputFrame, std::to_string(m_result), cv::Point2i(10, 460), cv::HersheyFonts::FONT_HERSHEY_TRIPLEX, 5, cv::Scalar::all(255), 3);
    }
    // show currently drawn path
    else
	{
	    for (auto i = 0u; i + 1 < path.size(); ++i)
        {
            cv::line(outputFrame, path[i], path[i+1], cv::Scalar(0, 0, 255), 3);
        }
	}
}

void DigitVisualizer::setSimplifiedPath(const std::vector<cv::Point2f>& simplifiedPath)
{
    m_simplifiedPath = simplifiedPath;
}

void DigitVisualizer::setResult(const int result)
{
    m_result = result;
}
