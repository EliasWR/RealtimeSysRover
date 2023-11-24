#ifndef REALTIMESYSROVER_OBJECT_DETECTION_HPP
#define REALTIMESYSROVER_OBJECT_DETECTION_HPP


#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>
#include "helpers/read_file_helper.hpp"

class ObjectDetection {
public:
    ObjectDetection () = default;
    ObjectDetection(const std::string& model, const std::string& config);
    static void preprocess(const cv::Mat& frame, cv::Mat& blob);
    void runModel(const cv::Mat& blob, std::vector<cv::Mat>& outputs);
    static void postprocess(const std::vector<cv::Mat>& outputs, const cv::Mat& frame, std::vector<int>& classIds, std::vector<float>& confidences, std::vector<cv::Rect>& boxes);
    void drawDetections(cv::Mat& frame, const std::vector<int>& classIds, const std::vector<float>& confidences, const std::vector<cv::Rect>& boxes);
    cv::Mat detectObjects(cv::Mat& frame);
private:
    cv::dnn::Net _net;
    const std::string _categoryPath = "../../yolo/coco.names";
    std::vector<std::string> _classNames = loadFileToVector(_categoryPath);
};

#endif //REALTIMESYSROVER_OBJECT_DETECTION_HPP
