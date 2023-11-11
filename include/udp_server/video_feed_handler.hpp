#ifndef VIDEO_FEED_HANDLER_HPP
#define VIDEO_FEED_HANDLER_HPP

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;

class VideoFeedHandler {
public:
    VideoFeedHandler ();
    void displayFrame (std::string frame);
    ~VideoFeedHandler();

private:
    std::jthread _thread;
    VideoFeed _video_feed;
};

#endif // VIDEO_FEED_HANDLER_HPP
