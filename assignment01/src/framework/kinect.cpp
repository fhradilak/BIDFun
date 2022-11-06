#include "kinect.hpp"

#include <iostream>
#include <cstdlib>

void printVideoMode(const openni::VideoMode & videoMode)
{
    std::cout << "Video mode:" << std::endl <<
        "\tWidth       : " << videoMode.getResolutionX() << std::endl <<
        "\tHeight      : " << videoMode.getResolutionY() << std::endl <<
        "\tFPS         : " << videoMode.getFps() << std::endl <<
        "\tPixelFormat : " << videoMode.getPixelFormat() << std::endl;
}

bool assertVideoMode(openni::VideoStream & stream, const int width, const int height, const int fps, const openni::PixelFormat pixelFormat)
{
    auto videoMode = stream.getVideoMode();
    printVideoMode(videoMode);
    auto needsUpdate = false;

    if(videoMode.getResolutionX() != width || videoMode.getResolutionY() != height)
    {
        std::cout << "Wrong resolution (should be " << width << "x" << height << "). Trying to fix..." << std::endl;
        videoMode.setResolution(width, height);
        needsUpdate = true;
    }

    if(videoMode.getFps() != fps)
    {
        std::cout << "Wrong FPS (should be " << fps << "). Trying to fix..." << std::endl;
        videoMode.setFps(fps);
        needsUpdate = true;
    }

    if(videoMode.getPixelFormat() != pixelFormat)
    {
        std::cout << "Wrong pixel format (should be " << pixelFormat << "). Trying to fix..." << std::endl;
        videoMode.setPixelFormat(pixelFormat);
        needsUpdate = true;
    }

    if(needsUpdate)
	{
        CHECK(stream.setVideoMode(videoMode), return false);
	}

    return true;
}

Kinect::Kinect(const char * device)
{
    // open device
    std::cout << "Using device " << (device == nullptr ? "NULL (default)" : device) << "..." << std::endl;
    CHECK(m_device.open(device), return);
    std::cout << "Opened device " << m_device.getDeviceInfo().getUri() << "." << std::endl << std::endl;

    // setup depth stream
    std::cout << "Creating depth stream..." << std::endl;
    CHECK(m_depthStream.create(m_device, openni::SENSOR_DEPTH), return);
    if(!assertVideoMode(m_depthStream, 640, 480, 30, openni::PIXEL_FORMAT_DEPTH_1_MM)) return;
    CHECK(m_depthStream.start(), std::cout << "Tip: Did you forget to plug the Kinect in?" << std::endl << std::endl; return);
    std::cout << "Depth stream successfully set up." << std::endl << std::endl;

    // setup color stream
    std::cout << "Creating color stream..." << std::endl;
    CHECK(m_colorStream.create(m_device, openni::SENSOR_COLOR), return);
    if(!assertVideoMode(m_colorStream, 640, 480, 30, openni::PIXEL_FORMAT_RGB888)) return;
    CHECK(m_colorStream.start(), std::cout << "Tip: Did you forget to plug the Kinect in?" << std::endl << std::endl; return);
    std::cout << "Color stream successfully set up." << std::endl << std::endl;

    // successfully set up
    m_ready = true;
}

Kinect::~Kinect()
{
    m_ready = false;

    m_depthStream.stop();
    m_depthStream.destroy();

    m_colorStream.stop();
    m_colorStream.destroy();

    m_device.close();
}

void Kinect::initialize()
{
    // set up OpenNI framework
    CHECK(openni::OpenNI::initialize(), std::exit(0));
}

void Kinect::listDevices()
{
    // fetch device list
    openni::Array<openni::DeviceInfo> deviceList;
	openni::OpenNI::enumerateDevices(&deviceList);

    // print header, check if any
    std::cout << "Found OpenNI devices:" << std::endl;
    if(deviceList.getSize() == 0)
    {
        std::cout << "\t--- None ---" << std::endl;
    }

    // enumerate list and print
	for (auto i = 0; i < deviceList.getSize(); ++i)
	{
        auto device = deviceList[i];
        std::cout << "\t" <<device.getName() << " with URI " << device.getUri() << std::endl;
	}

    // empty line afterwards
    std::cout << std::endl;
}

bool Kinect::getColorFrame(cv::Mat& frame)
{
    // get OpenNI frame data
    openni::VideoFrameRef niFrame;
    CHECK(m_colorStream.readFrame(&niFrame), return false);
    const auto imageBuffer = static_cast<const openni::RGB888Pixel*>(niFrame.getData());
    // copy to OpenCV frame
    frame.create(niFrame.getHeight(), niFrame.getWidth(), CV_8UC3);
	memcpy(frame.data, imageBuffer, niFrame.getHeight() * niFrame.getWidth() * 3 * sizeof(uint8_t));
    // fix color channels
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    // fix orientation for downwards-pointing usage
    cv::flip(frame, frame, 0);
    // signalize success
    return true;
}

bool Kinect::getDepthFrame(cv::Mat& frame)
{
    // get OpenNI frame data
    openni::VideoFrameRef niFrame;
    CHECK(m_depthStream.readFrame(&niFrame), return false);
    const auto imageBuffer = static_cast<const openni::DepthPixel*>(niFrame.getData());
    // copy to OpenCV frame
    frame.create(niFrame.getHeight(), niFrame.getWidth(), CV_16UC1);
	memcpy(frame.data, imageBuffer, niFrame.getHeight() * niFrame.getWidth() * sizeof(uint16_t));
    // fix orientation for downwards-pointing usage
    cv::flip(frame, frame, 0);
    // signalize success
    return true;
}

bool Kinect::ready() const
{
    return m_ready;
}
