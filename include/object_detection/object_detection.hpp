#ifndef REALTIMESYSROVER_OBJECT_DETECTION_HPP
#define REALTIMESYSROVER_OBJECT_DETECTION_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>
#include "helpers/read_file_helper.hpp"
#include "helpers/detection_helper.hpp"

class ObjectDetection {
public:
    ObjectDetection();
    static void preprocess(const cv::Mat& frame, cv::Mat& blob);
    void runModel(const cv::Mat& blob, std::vector<cv::Mat>& outputs);
    void postprocess(const std::vector<cv::Mat>& outputs, const cv::Mat& frame, Detection& detection);
    cv::Mat drawDetections (cv::Mat& frame, std::optional<Detection>& detection);
    Detection detectObjects(const cv::Mat& frame);
    std::optional<Detection> getLatestDetection ();
    void run ();
    void stop ();
    void addLatestFrame(const cv::Mat& frame);

    bool _running{false};

private:
    cv::dnn::Net _net;
    std::string _categoryPath;
    std::string _modelPath;
    std::string _configPath;
    std::vector<std::string> _classNames;

    std::thread _t;
    cv::Mat _latest_frame;
    std::mutex mutex;
    std::condition_variable cv;
    bool new_frame_available = false;
    Detection _latest_detection;
    std::chrono::steady_clock::time_point _last_detection_time;
    const int MAX_DETECTION_AGE = 1000; //seconds
};

#endif //REALTIMESYSROVER_OBJECT_DETECTION_HPP
