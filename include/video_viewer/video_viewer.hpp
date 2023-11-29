#ifndef REALTIMESYSROVER_VIDEO_VIEWER_HPP
#define REALTIMESYSROVER_VIDEO_VIEWER_HPP

#include "opencv2/opencv.hpp"


#include <thread>
#include <mutex>
#include <atomic>
#include <queue>

class VideoViewer{
public:
  VideoViewer();
  ~VideoViewer();
  void addFrame(const cv::Mat& frame);
  void display();
  bool accept_frames{false};

private:
  std::queue<cv::Mat> frame_queue;
  std::mutex queue_mutex;
  cv::Mat dummy_frame;
  std::chrono::steady_clock::time_point last_frame_time;
  cv::Mat getLatestFrame();
};


#endif//REALTIMESYSROVER_VIDEO_VIEWER_HPP
