#include "../framework/kinect.hpp"
#include "touchRecognizer.hpp"
#include "touchVisualizer.hpp"
#include "drawVisualizer.hpp" // only for bonus assignment

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

int main()
{
	std::cout << "Hello World!";

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

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    cv::Mat touchFrame;
	cv::Mat frame;
    auto running = true;

    // calibrate
    
	kinect.getDepthFrame(depthFrame);
	touchRecognizer.calibrate(depthFrame);

    // prepare windows - this isn't necessary, but it allows to assign useful positions
   // cv::namedWindow("color");
	//cv::namedWindow("depth");
	
	//cv::namedWindow("color", CV_WINDOW_AUTOSIZE);
	

    /*~~~~~~~~~~~~~~~*
     * DEBUG WINDOWS *
     * COULD GO HERE *
     *~~~~~~~~~~~~~~~*/

    // move windows
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);
    /*~~~~~~~~~~~~~~~*
     * DEBUG WINDOWS *
     * COULD GO HERE *
     *~~~~~~~~~~~~~~~*/
	
	std::vector<cv::RotatedRect> allPositions;
    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);
        colorFrame.copyTo(touchFrame);

		
		int c = cv::waitKey(30);

		switch (c) {
		case 'q':
			break;
		}


        // run touch recognizer
        
        /*~~~~~~~~~~~*
         * YOUR CODE *
         * GOES HERE *
         *~~~~~~~~~~~*/

		std::vector<cv::RotatedRect> positions;
		positions = touchRecognizer.recognize(depthFrame);
		cv::Scalar color = cv::Scalar(255, 255, 0);
		allPositions.insert(allPositions.end(),positions.begin(),positions.end());

		if (allPositions.size() > 0) {
			for (int i = 0; i < allPositions.size(); i++) {
			
				cv::circle(colorFrame, allPositions[i].center, 10, color,5);
				
			}
		}


        // run visualizer - there may be no position
        
        /*~~~~~~~~~~~*
         * YOUR CODE *
         * GOES HERE *
         *~~~~~~~~~~~*/

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        //cv::imshow("depth", depthFrame);
        //cv::imshow("color", colorFrame);
		//cv::imshow("touch", touchFrame);

        // check for keypresses
        const auto key = cv::waitKey(10);
        switch (key)
        {
		case 0x1b: // esc - exit
            running = false;
            break;
		case 0x0d: // enter - calibrate again
            /*~~~~~~~~~~~*
             * YOUR CODE *
             * GOES HERE *
             *~~~~~~~~~~~*/
            break;
        default:
            break;
        }
    }
}
