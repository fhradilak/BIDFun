#include "touchRecognizer.hpp"

#include <algorithm>

void TouchRecognizer::calibrate(const cv::Mat& background)
{
	m_background = background.clone();
	
}

std::vector<cv::RotatedRect> TouchRecognizer::recognize(const cv::Mat& depthFrame) const
{
	cv::Mat result;
	cv::absdiff(m_background, depthFrame, result);

	const int MAX_THRESH = 256;

	//result= abs(result-MAX_THRESH);
	result.convertTo(result, CV_8UC1);
	cv::threshold(result, result, 50, MAX_THRESH,cv::THRESH_TOZERO_INV);
	//cv::imshow("inverted0", result);
	cv::threshold(result, result, 20, MAX_THRESH, cv::THRESH_BINARY);
	cv::blur(result, result, cv::Size(8, 8));
	//cv::imshow("inverted", result);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	findContours(result, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //create contours
	std::vector<cv::RotatedRect> minRect(contours.size());
	std::vector<cv::RotatedRect> minEllipse(contours.size());

	for (int i = 0; i < contours.size(); i++) //fit ellipse and rectangle
	{
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
		if (contours[i].size() > 20)
		{
			minEllipse[i] = cv::fitEllipse(cv::Mat(contours[i]));
		}
	}

	std::vector<cv::RotatedRect> filteredEllipse;
	std::vector<cv::RotatedRect> filteredRect;
	const int SIZE_THRESHOLD = 0;
	for (int i = 0; i < minEllipse.size(); i++) {
		int ellipseWidth = minEllipse[i].size.width;
		int ellipseHight = minEllipse[i].size.height;
		
		if (ellipseHight * ellipseWidth > SIZE_THRESHOLD) {
			filteredEllipse.push_back(minEllipse[i]);
			filteredRect.push_back(minRect[i]);
		}
	}


	cv::Mat drawing = cv::Mat::zeros(result.size(), CV_8UC3);
	for (int i = 0; i < filteredEllipse.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255, 0, 0);
		// ellipse
		cv::ellipse(drawing, filteredEllipse[i], color, 2, 8);
		// rotated rectangle
		cv::Point2f rect_points[4];
		filteredRect[i].points(rect_points);
		for (int j = 0; j < 4; j++)
			cv::line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
		
	}
	//cv::imshow("Contours", drawing);


	return filteredEllipse;
}
