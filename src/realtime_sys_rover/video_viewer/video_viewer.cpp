#include "video_viewer/video_viewer.hpp"

/**
 * @brief Construct a new Video Viewer object
 *
 * @param screen_name
 */
VideoViewer::VideoViewer(std::string screen_name) :
    _dummy_frame(cv::Mat::zeros(cv::Size(640, 480), CV_8UC3)),
    _last_frame_time(std::chrono::steady_clock::now()),
    _screen_name(screen_name) {
  cv::putText(_dummy_frame, "Waiting for video feed...", cv::Point(50, 240),
              cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);
}

/**
 * @brief Destructs the Video Viewer object
 *
 * @details
 * Destroys the window.
 */
VideoViewer::~VideoViewer() {
  cv::destroyWindow(_screen_name);
}

/**
 * @brief Adds a frame to the frame queue.
 *
 * @param frame The frame to add.
 */
void VideoViewer::addFrame(const cv::Mat &frame) {
  std::unique_lock<std::mutex> guard(_queue_mutex);
  if (!frame.empty()) {
    if (!_frame_queue.empty()) {
      _frame_queue.pop();
    }
    _frame_queue.push(frame);
    _last_frame_time = std::chrono::steady_clock::now();
  }
}

/**
 * @brief Displays the latest frame.
 *
 */
void VideoViewer::display() {
  cv::Mat frame = getLatestFrame();
  cv::imshow(_screen_name, frame);

  cv::waitKey(1);
}

/**
 * @brief Gets the latest frame.
 *
 * @return cv::Mat The latest frame.
 */
cv::Mat VideoViewer::getLatestFrame() {
  std::unique_lock<std::mutex> guard(_queue_mutex);
  auto now = std::chrono::steady_clock::now();
  cv::Mat frame;
  if (!_frame_queue.empty()) {
    frame = _frame_queue.front();
    if (now - _last_frame_time >= std::chrono::seconds(3)) {
      _frame_queue.pop();
    }
  }
  else {
    frame = _dummy_frame;
  }
  return frame;
}