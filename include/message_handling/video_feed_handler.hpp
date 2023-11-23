#ifndef VIDEO_FEED_HANDLER_HPP
#define VIDEO_FEED_HANDLER_HPP

#include <iostream>
#include "boost/asio.hpp"
#include "opencv2/opencv.hpp"
#include "message_handling/message_handler.hpp"
#include "object_detection/object_detection.hpp"
#include "my_messages.pb.h"

using udp = boost::asio::ip::udp;

class VideoFeedHandler : public MessageHandler {
public:
    VideoFeedHandler ();
    VideoFeedHandler (const std::string& model, const std::string& config);
    void openWindow () override;
    [[maybe_unused]] void displayFrame (const std::vector<char>& frame, size_t& len);
    void displayFrameDetections (const std::vector<char>& frame, size_t& len);
    void handleMessage (const std::vector<char>& message, size_t& len) override;
    ~VideoFeedHandler();

private:
    VideoFeed _video_feed;
    ObjectDetection _object_detector;
};

#endif // VIDEO_FEED_HANDLER_HPP
