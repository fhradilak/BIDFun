#include "../framework/kinect.hpp"
#include "../assignment01/touchRecognizer.hpp"
#include "classifier.hpp"
#include "digitVisualizer.hpp"

#include <opencv2/opencv.hpp>
#include "data.hpp"

int main()
{
    // initialize framework
    Kinect::initialize();
    Kinect::listDevices();

    // set up device
    Kinect kinect;
    if(!kinect.ready())
    {
        std::cout << "Could not set up Kinect. Exiting." << std::endl;
        return -1;
    }

    // prepare touch pad
    TouchRecognizer touchRecognizer;

    // load data set
    const auto data = DataSet::read("pendigits.tra");

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    cv::Mat outputFrame;
    auto running = true;
    std::vector<cv::Point2f> path;

    // calibrate
    kinect.getDepthFrame(depthFrame);
    touchRecognizer.calibrate(depthFrame);

    // prepare classifier and viewer
    Classifier classifier(data, 25);
    DigitVisualizer digitVisualizer;

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");
    cv::namedWindow("output");
	cv::namedWindow("colorTest");

    // move windows
    kinect.getColorFrame(colorFrame);
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);
    cv::moveWindow("output", 0 * xOffset, 0 * yOffset);
	cv::moveWindow("colorTest", 1 * xOffset, 0 * yOffset);

    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);

        // run touch recognizer
        auto positions = touchRecognizer.recognize(depthFrame);
		double pathLength=0.0;
        if(!positions.empty())
		{
            // track user's drawing using the path variable
			for (int i = 0; i < positions.size(); i++) {
				path.push_back(positions[i].center);
			}
		} 
        else
		{
			if (path.size() > 8) {
				classifier.classify(path);				
			}
			path.clear();

			// after classifying, you should update the visualizer like this:
			// digitVisualizer.setSimplifiedPath(classifier.getSimplifiedPath());
			// digitVisualizer.setResult(classifier.getResult());	
		}
		

			
			



		//Frnaziskas Idee
				/*
			float partDistance = distance / 8;
			float currDistance=partDistance;
			std::vector<cv::Point2f> filteredPoints;
			filteredPoints.push_back(path[1]);
			int cPI = 0; //currentPointIndex
			cv::Point2f vector;
			cv::Point2f newPoint;
			float cPIDistance = cv::norm(filteredPoints[cPI + 1] - filteredPoints[cPI]);



			while (currDistance < distance) {
				while (partDistance > cPIDistance) {
					cPIDistance += cv::norm(filteredPoints[cPI + 2] - filteredPoints[cPI + 1]);
					cPI++;
				}
				vector = filteredPoints[cPI + 1] - filteredPoints[cPI];
				float length = cv::norm(vector);
				float missingLength = partDistance - (cPIDistance - length);
				newPoint = vector / length * missingLength + filteredPoints[cPI];
				filteredPoints[cPI] = newPoint;
				cPIDistance = cv::norm(filteredPoints[cPI + 1] - filteredPoints[cPI]);
				filteredPoints.push_back(newPoint);
				currDistance += partDistance;
				partDistance = distance / 8;
			}
			*/


	

        // visualize results
        digitVisualizer.draw(colorFrame, path, outputFrame);

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);
        cv::imshow("color", colorFrame);
        cv::imshow("output", outputFrame);
		cv::imshow("colorTest", colorFrame);

        // check for keypresses
        const auto key = cv::waitKey(10);
        switch (key)
        {
		case 0x1b: // esc
            running = false;
            break;
		case 0x0d: // enter
            touchRecognizer.calibrate(depthFrameUnscaled);
            break;
		case 0x30: // number keys - you can use this to take a look at the data set
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39: 
            DataVisualizer::showNumber(data, key - 0x30);
            break;
        default:
            break;
        }
    }
}
