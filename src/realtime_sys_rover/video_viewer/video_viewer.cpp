#include "video_viewer/video_viewer.hpp"

/**
 * @brief Construct a new Video Viewer object
 *
 * @param screen_name
 */
VideoViewer::VideoViewer(std::string screen_name) :
    dummy_frame(cv::Mat::zeros(cv::Size(640, 480), CV_8UC3)),
    last_frame_time(std::chrono::steady_clock::now()),
    _screen_name(screen_name) {
  cv::putText(dummy_frame, "Waiting for video feed...", cv::Point(50, 240),
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
  std::unique_lock<std::mutex> guard(queue_mutex);
  if (!frame.empty()) {
    accept_frames = false;
    if (!frame_queue.empty()) {
      frame_queue.pop();
    }
    frame_queue.push(frame);
    last_frame_time = std::chrono::steady_clock::now();
  }
}

/**
 * @brief Displays the latest frame.
 *
 */
void VideoViewer::display() {
  cv::Mat frame{getLatestFrame()};
  cv::imshow(_screen_name, frame);

  cv::waitKey(1);
}

/**
 * @brief Gets the latest frame.
 *
 * @return cv::Mat The latest frame.
 */
cv::Mat VideoViewer::getLatestFrame() {
  std::unique_lock<std::mutex> guard(queue_mutex);
  auto now = std::chrono::steady_clock::now();
  cv::Mat frame;
  if (!frame_queue.empty()) {
    frame = frame_queue.front();
    if (now - last_frame_time >= std::chrono::seconds(3)) {
      frame_queue.pop();
    }
  }
  else {
    frame = dummy_frame;
  }
  return frame;
}