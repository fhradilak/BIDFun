#pragma once

#include <opencv2/opencv.hpp>
#include <OpenNI.h>

#define CHECK(call, ifFailed) { auto status = call; if(status != openni::Status::STATUS_OK){ std::cout << "Error while running " << #call << ": " << status << " - " << openni::OpenNI::getExtendedError() <<std::endl; ifFailed; }};

class Kinect
{
public:
    /**
     * \brief Set up the Kinect and the video streams. Requires initialize() to be called beforehand.
     * \param device The device URI. Leave blank to use the default device.
     */
    explicit Kinect(const char * device = openni::ANY_DEVICE);
    /**
     * \brief Close the video streams and release the Kinect.
     */
    ~Kinect();
    /**
     * \brief Initialize the underlying OpenNI framework. Needs to be called before the Kinect can be used.
     */
    static void initialize();
    /**
     * \brief Log the available OpenNI devices.
     */
    static void listDevices();
    /**
     * \brief Get the current frame from the color video stream.
     * \param frame A cv::Mat which will contain the frame afterwards.
     * \return True if the frame was successfully retrieved, false otherwise.
     */
    bool getColorFrame(cv::Mat& frame);
    /**
     * \brief Get the current frame from the depth video stream.
     * \param frame A cv::Mat which will contain the frame afterwards.
     * \return True if the frame was successfully retrieved, false otherwise.
     */
    bool getDepthFrame(cv::Mat& frame);
    /**
     * \brief Checks if the Kinect was successfully set up.
     * \return True if the Kinect is set up correctly, false otherwise.
     */
    bool ready() const;
private:
    bool m_ready = false;
    openni::Device m_device;
    openni::VideoStream m_depthStream;
    openni::VideoStream m_colorStream;
};
