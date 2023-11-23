#include "object_detection/object_detection.hpp"


ObjectDetection::ObjectDetection(const std::string& model, const std::string& config) :
        _net(cv::dnn::readNet(model, config)) {
    std::vector<std::string> classNames = loadFileToVector(_categoryPath);
    std::cout << "Loaded " << classNames.size() << " classes." << std::endl;
}

void ObjectDetection::preprocess(const cv::Mat &frame, cv::Mat &blob) {
    double scalefactor = 1/255.0;
    cv::Size size = cv::Size(416, 416);
    cv::Scalar mean = cv::Scalar(0, 0, 0);
    bool swapRB = true;
    bool crop = false;
    cv::dnn::blobFromImage(frame, blob, scalefactor, size, mean, swapRB, crop);
}

void ObjectDetection::runModel(const cv::Mat &blob, std::vector<cv::Mat> &outputs) {
    _net.setInput(blob);
    _net.forward(outputs, _net.getUnconnectedOutLayersNames());
}

void ObjectDetection::postprocess(const std::vector<cv::Mat> &outputs, const cv::Mat &frame, std::vector<int> &classIds,
                                  std::vector<float> &confidences, std::vector<cv::Rect> &boxes) {
    double detection_threshold = 0.3;
    for (auto& output : outputs) {
        auto* data = reinterpret_cast<float*>(output.data);
        for (int j = 0; j < output.rows; ++j, data += output.cols) {
            cv::Mat scores = output.row(j).colRange(5, output.cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
            if (confidence > detection_threshold) {
                int centerX = static_cast<int>(data[0] * frame.cols);
                int centerY = static_cast<int>(data[1] * frame.rows);
                int width = static_cast<int>(data[2] * frame.cols);
                int height = static_cast<int>(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back(static_cast<float>(confidence));
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }
}
/*
void ObjectDetection::drawDetections(cv::Mat &frame, const std::vector<int> &classIds,
                                     const std::vector<float> &confidences, const std::vector<cv::Rect> &boxes) {
    std::vector<std::string> classNames = loadFileToVector(_categoryPath);
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 3);
        std::string label = "";
        if (classIds[idx] < classNames.size()) {
            label = classNames[classIds[idx]]; // Get the name of the label
        } else {
            std::cerr << "Class ID " << classIds[idx] << " is out of range." << std::endl;
        }
        label += ": " + cv::format("%.2f", confidences[idx]);

        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        int top = std::max(box.y, labelSize.height);
        cv::rectangle(frame, cv::Point(box.x, top - labelSize.height),
                      cv::Point(box.x + labelSize.width, top + baseLine),
                      cv::Scalar(255, 255, 255), cv::FILLED);
        cv::putText(frame, label, cv::Point(box.x, box.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
    }
}
*/

void ObjectDetection::drawDetections(cv::Mat &frame, const std::vector<int> &classIds,
                                     const std::vector<float> &confidences, const std::vector<cv::Rect> &boxes) {
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 3);
        std::string label = std::to_string(classIds[idx]) + ": " + std::to_string(confidences[idx]);
        cv::putText(frame, label, cv::Point(box.x, box.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
    }
}

cv::Mat ObjectDetection::detectObjects(cv::Mat& frame) {
    cv::Mat blob;
    std::vector<cv::Mat> outputs;
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    preprocess(frame, blob);
    runModel(blob, outputs);
    postprocess(outputs, frame, classIds, confidences, boxes);
    drawDetections(frame, classIds, confidences, boxes);

    return frame;
}