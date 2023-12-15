#ifndef REALTIMESYSROVER_VIDEO_VIEWER_HPP
#define REALTIMESYSROVER_VIDEO_VIEWER_HPP

#include "opencv2/opencv.hpp"

#include <atomic>
#include <mutex>
#include <queue>

class VideoViewer {
public:
  VideoViewer(std::string screen_name = "Video Viewer");
  ~VideoViewer();
  void addFrame(const cv::Mat &frame);
  void display();

private:
  std::queue<cv::Mat> _frame_queue;
  std::mutex _queue_mutex;
  cv::Mat _dummy_frame;
  std::chrono::steady_clock::time_point _last_frame_time;
  cv::Mat getLatestFrame();
  std::string _screen_name;
};


#endif//REALTIMESYSROVER_VIDEO_VIEWER_HPP
