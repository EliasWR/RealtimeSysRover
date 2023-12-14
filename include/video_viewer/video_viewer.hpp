#ifndef REALTIMESYSROVER_VIDEO_VIEWER_HPP
#define REALTIMESYSROVER_VIDEO_VIEWER_HPP

#include "opencv2/opencv.hpp"

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

class VideoViewer {
public:
  VideoViewer(std::string screen_name = "Video Viewer");
  ~VideoViewer();
  void addFrame(const cv::Mat &frame);
  void display();
  bool accept_frames{false};

private:
  std::queue<cv::Mat> frame_queue;
  std::mutex queue_mutex;
  cv::Mat dummy_frame;
  std::chrono::steady_clock::time_point last_frame_time;
  cv::Mat getLatestFrame();
  std::string _screen_name;
};


#endif//REALTIMESYSROVER_VIDEO_VIEWER_HPP
