#ifndef REALTIMESYSROVER_OBJECT_DETECTION_HPP
#define REALTIMESYSROVER_OBJECT_DETECTION_HPP

#include <condition_variable>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "helpers/detection_helper.hpp"
#include "helpers/read_file_helper.hpp"

class ObjectDetection {
public:
  ObjectDetection();
  static void preprocess(const cv::Mat &frame, cv::Mat &blob);
  void runModel(const cv::Mat &blob, std::vector<cv::Mat> &outputs);
  void postprocess(const std::vector<cv::Mat> &outputs, const cv::Mat &frame, Detection &detection);
  cv::Mat drawDetections(cv::Mat &frame, std::optional<Detection> &detection);
  Detection detectObjects(const cv::Mat &frame);
  std::optional<Detection> getLatestDetection();
  void run();
  void stop();
  void addLatestFrame(const cv::Mat &frame);
  bool running = false;

private:
  cv::dnn::Net _net;
  std::string _category_path;
  std::string _model_path;
  std::string _config_path;
  std::vector<std::string> _class_names;

  std::thread _t;
  cv::Mat _latest_frame;
  std::mutex _mutex;
  std::condition_variable _cv;
  bool _new_frame_available = false;
  Detection _latest_detection;
  std::chrono::steady_clock::time_point _last_detection_time;
  const int _MAX_DETECTION_AGE = 1000;//seconds
};

#endif//REALTIMESYSROVER_OBJECT_DETECTION_HPP
