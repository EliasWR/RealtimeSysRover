#ifndef REALTIMESYSROVER_Detection_HPP
#define REALTIMESYSROVER_Detection_HPP

#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

struct Detection {
  std::vector<cv::Rect> boxes;
  std::vector<float> confidences;
  std::vector<int> class_ids;
  std::pair<int, int> frame_size;
};

#endif//REALTIMESYSROVER_Detection_HPP