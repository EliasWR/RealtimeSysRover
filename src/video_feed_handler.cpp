#include "udp_server/Video_Feed_Handler.hpp"

VideoFeedHandler::VideoFeedHandler() : _video_feed(VideoFeed()){};

void VideoFeedHandler::openWindow() {
    cv::namedWindow("VideoFeed", cv::WINDOW_AUTOSIZE);
}

void VideoFeedHandler::displayFrame (const std::vector<char>& frame, size_t& len) {
    _video_feed.ParseFromArray(frame.data(), len);

    std::vector<uchar> encoded_frame(_video_feed.messagefeed().begin(), _video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);

    if (!decoded_frame.empty()) {
        cv::imshow("VideoFeed", decoded_frame);
        cv::waitKey(1);
    } else {
        std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
    }
}

void VideoFeedHandler::handleMessage(const std::vector<char>& message, size_t& len) {
    displayFrame(message, len);
}

VideoFeedHandler::~VideoFeedHandler () {
    cv::destroyWindow("VideoFeed");
}