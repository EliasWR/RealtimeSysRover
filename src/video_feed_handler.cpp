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

/*
    VideoFeed video_feed;
    video_feed.ParseFromArray(recv_buffer.data(), len);

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);

    if (!decoded_frame.empty()) {
        cv::imshow("VideoFeed", decoded_frame);
    } else {
        std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
    }

 */

/*
 // Threader
 _thread = std::jthread([&] {
        try {
            while (true) {

                _video_feed.ParseFromArray();
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught in thread: " << e.what() << '\n';
        } catch (...) {
            std::cerr << "Unknown exception caught in thread.\n";
        }
    });
 */

/*
VideoFeedHandler::VideoFeedHandler() = default;

VideoFeedHandler::MessageHandler VideoFeedHandler::getHandler() {
    return [this](const std::vector<char>& data, const udp::endpoint& remote_endpoint) {
        this->handleVideoFeed(data, remote_endpoint);
    };
}

void VideoFeedHandler::handleVideoFeed(const std::vector<char>& data, const udp::endpoint& remote_endpoint) {
    VideoFeed video_feed;
    video_feed.ParseFromArray(data.data(), data.size());

    std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
    cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);

    if (!decoded_frame.empty()) {
        cv::imshow("VideoFeed", decoded_frame);
        cv::waitKey(1); // This is needed to process the window events.
    } else {
        std::cerr << "Decoded frame is empty or cannot be decoded." << std::endl;
    }
}
*/