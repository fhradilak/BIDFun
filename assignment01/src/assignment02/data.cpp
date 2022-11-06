#include "data.hpp"

#include <utility>
#include <fstream>
#include <sstream>
#include <limits>

#include <opencv2/opencv.hpp>

Digit::Digit(std::vector<cv::Point2i> points, const int label)
: m_points(std::move(points))
, m_label(label)
{
}

const std::vector<cv::Point2i>& Digit::points() const
{
    return m_points;
}

int Digit::label() const
{
    return m_label;
}

std::vector<Digit> DataSet::read(const std::string& fileName, const int numberOfRows)
{
    auto result = std::vector<Digit>();

    // open data file
    std::ifstream file;
    file.open(fileName);

    if (!file.is_open())
    {
        std::cout << "Could not open data file. Exiting." << std::endl;
        exit(-1);
    }

    // split lines
    std::string line;
    auto lineIndex = 0;
    while (lineIndex < numberOfRows && std::getline(file, line))
    {
        auto points = std::vector<cv::Point2i>();

        auto lineStream = std::stringstream(line);

        std::string valueString;
        auto index = -1;
        int xCoord;

        // split values
        while (std::getline(lineStream, valueString, ','))
        {
            index++;
            auto value = std::stoi(valueString);

            // last value is label -> path done
            if (index == 16)
            {
                result.emplace_back(points, value);
                break;
            }

            // even ones are x -> store
            if (index % 2 == 0)
            {
                xCoord = value;
                continue;
            }

            // odd ones are y -> point done
            points.emplace_back(xCoord, value);
        }

        lineIndex++;
    }

    return result;
}

std::vector<Digit> DataSet::read(const std::string& fileName)
{
    // auto-overloading doesn't work, since numeric_limits seems to have problems in headers, idk
    return read(fileName, std::numeric_limits<int>::max());
}

void DataVisualizer::showNumber(std::vector<Digit> dataSet, const int number, const int scale)
{
    // prepare canvas
    cv::Mat dataVisFrame;
    dataVisFrame.create(100 * scale, 100 * scale, CV_8UC3);
    dataVisFrame = cv::Scalar::all(0);

    // for all digits
    for (const auto& digit : dataSet)
    {
        // filter by label
        if (digit.label() != number) continue;

        auto points = digit.points();
        const auto size = points.size();

        // draw line with color gradient from blue (start) to red (end)
        for (auto i = 0u; i + 1 < size; ++i)
        {
            cv::line(dataVisFrame, points[i] * scale, points[i+1] * scale, cv::Scalar((size - i) * 255.0 / size, 0, (i + 1.0) * 255.0 / size));
        }
    }

    // OpenCV coordinate system has positive y going down
    cv::flip(dataVisFrame, dataVisFrame, 0);

    // place window and show
    const auto name = "data " + std::to_string(number);
    cv::namedWindow(name);
    cv::moveWindow(name, (number % 5) * 100 * scale, number / 5 * 100 * scale);
    cv::imshow(name, dataVisFrame);
}
