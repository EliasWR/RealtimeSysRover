#ifndef VIDEO_FEED_HANDLER_HPP
#define VIDEO_FEED_HANDLER_HPP

#include <iostream>
#include <functional>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include "message_handling/message_handler.hpp"
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;

class VideoFeedHandler : public MessageHandler {
public:
    VideoFeedHandler ();
    void displayFrame (std::string& frame);
    virtual void handleMessage(const std::string& message) override;
    ~VideoFeedHandler();

private:
    std::unique_ptr<VideoFeed> _video_feed;
};

#endif // VIDEO_FEED_HANDLER_HPP
