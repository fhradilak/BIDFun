#include "../framework/kinect.hpp"

#include <iostream>

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

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    auto running = true;

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");

    // move windows
    kinect.getColorFrame(colorFrame);
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);

    while(running)
    {
        // fetch and display depth frame
        kinect.getDepthFrame(depthFrame);
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);

        // fetch and display color frame
        kinect.getColorFrame(colorFrame);
        cv::imshow("color", colorFrame);

        // check for esc
		const auto key = cv::waitKey(10);
        switch (key)
        {
		case 0x1b: // esc
            running = false;
            break;
        default: 
            break;
        }
    }
}
