#include "classifier.hpp"
#include <utility>
#include <vector>
#include <algorithm>

Classifier::Classifier(std::vector<Digit> dataSet, const int k)
: c_dataSet(std::move(dataSet))
, c_simplifiedSize(c_dataSet.front().points().size())
, c_k(k)
{
}

void Classifier::classify(const std::vector<cv::Point2f>& path)
{
	cv::namedWindow("Drawing");
    // equidistant sampling
    simplify(path);
	
	float minX = m_simplifiedPath[0].x;
	float maxY = m_simplifiedPath[0].x;
	float minY= m_simplifiedPath[0].x;
	float maxX = m_simplifiedPath[0].x;

	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		if (m_simplifiedPath[i].x < minX)
			minX = m_simplifiedPath[i].x;
		if (m_simplifiedPath[i].y < minY)
			minY = m_simplifiedPath[i].y;
		if (m_simplifiedPath[i].x > maxX)
			maxX = m_simplifiedPath[i].x;
		if (m_simplifiedPath[i].y > maxY)
			maxY = m_simplifiedPath[i].y;
	}
	
	cv::Mat drawing = cv::Mat::zeros(cv::Size(maxX-minX, maxY-minY), CV_64FC1);
	cv::Scalar color = cv::Scalar(255, 255, 255);

	for (int i = 0; i < m_simplifiedPath.size(); i++) {
		m_simplifiedPath[i].x -= minX;
		m_simplifiedPath[i].y -= minY;
	}
	for (int i = 0; i < m_simplifiedPath.size()-1; i++) {
		cv::line(drawing, m_simplifiedPath[i], m_simplifiedPath[i + 1], color, 5);
	}
	resize(drawing, drawing, cv::Size(100, 100),cv::INTER_LINEAR);
	cv::flip(drawing, drawing, 1);
	cv::imshow("Drawing", drawing);
    // normalize and mirror y

    /*~~~~~~~~~~~*
     * YOUR CODE *
     * GOES HERE *
     *~~~~~~~~~~~*/

    // match using knn
	int k = 3;
		

	for (int i = 0; i++; i < 0) {
		c_dataSet;
	}
    // you should store your result in m_result
    // m_result = ...
}

int Classifier::getResult() const
{
    return m_result;
}

std::vector<cv::Point2f> Classifier::getSimplifiedPath() const
{
    return m_simplifiedPath;
}

void Classifier::simplify(std::vector<cv::Point2f> path)
{
	int point_intervall = path.size() / 8;
	for (int i = 0; i < path.size(); i += point_intervall) {
		m_simplifiedPath.push_back(path[i]);
	}


	/*	for equal spacing (not working):
	for (int i = 0; i < path.size(); i++) {
		pathLength += cv::norm(path[i + 1] - path[i]);
	}

	int pointCount = 8;
	//std::vector<cv::Point2f> path;
	std::vector<cv::Point2f> newPath;

	float segmentlength = pathLength / pointCount;
	float currentPosition = 0;
	float nextPointPosition = 0;
	float prevPointPosition = 0;
	int cpi = 0;

	while (pathLength >= currentPosition)
	{
		if (nextPointPosition == currentPosition) {
			newPath.push_back(path[cpi]);
			currentPosition += segmentlength;
			continue;
		}
		else if (nextPointPosition < currentPosition) {
			prevPointPosition = nextPointPosition;
			nextPointPosition += cv::norm(path[cpi + 1] - path[cpi]);
			cpi++;
			continue;
		}
		float distFromPrevPoint = currentPosition - prevPointPosition;
		cv::Point2f normVector = path[cpi] - path[cpi - 1];
		normVector = normVector / cv::norm(normVector);
		newPath.push_back(normVector * distFromPrevPoint + path[cpi - 1]);
		currentPosition += segmentlength;

		cv::Scalar color = cv::Scalar(100, 0, 50);
		cv::circle(colorFrame, newPath.back(), 10, color, 5);
	}*/
}
