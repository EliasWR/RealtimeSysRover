#include "message_handling/video_feed_handler.hpp"

VideoFeedHandler::VideoFeedHandler() : _video_feed(VideoFeed()){};

VideoFeedHandler::VideoFeedHandler(const std::string& model, const std::string& config)
        : _object_detector(model, config) {}

void VideoFeedHandler::openWindow() {
    cv::namedWindow("VideoFeed", cv::WINDOW_AUTOSIZE);
    cv::Mat dummyFrame = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
    cv::putText(dummyFrame, "Waiting for video feed...", cv::Point(50, 240),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::imshow("VideoFeed", dummyFrame);
    cv::waitKey(1);
}

[[maybe_unused]] void VideoFeedHandler::displayFrame (const std::vector<char>& frame, size_t& len) {
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

void VideoFeedHandler::displayFrameDetections (const std::vector<char>& frame, size_t& len) {
    _video_feed.ParseFromArray(frame.data(), len);

    std::vector<uchar> encoded_frame(_video_feed.messagefeed().begin(), _video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);

    if (!decoded_frame.empty()) {
        cv::Mat detections = _object_detector.detectObjects(decoded_frame);

        cv::imshow("VideoFeed", detections);
        cv::waitKey(1);
    } else {
        std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
    }
}

void VideoFeedHandler::handleMessage(const std::vector<char>& message, size_t& len) {
    // displayFrame(message, len);
    displayFrameDetections(message, len);
}

VideoFeedHandler::~VideoFeedHandler () {
    cv::destroyWindow("VideoFeed");
}