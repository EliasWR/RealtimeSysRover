#include "object_detection/object_detection.hpp"


ObjectDetection::ObjectDetection() {
    _categoryPath = "../../yolo/coco.names";
    _modelPath = "../../yolo/yolov3-tiny.weights";
    _configPath = "../../yolo/yolov3-tiny.cfg";
    _classNames = loadFileToVector(_categoryPath);
    _net = cv::dnn::readNet(_modelPath, _configPath);
}

void ObjectDetection::preprocess(const cv::Mat &frame, cv::Mat &blob) {
    double scale_factor = 1/255.0;
    cv::Size size = cv::Size(416, 416);
    cv::Scalar mean = cv::Scalar(0, 0, 0);
    bool swapRB = true;
    bool crop = false;
    cv::dnn::blobFromImage(frame, blob, scale_factor, size, mean, swapRB, crop);
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

cv::Mat ObjectDetection::drawDetections (cv::Mat &frame, const std::vector<int> &classIds,
                                     const std::vector<float> &confidences, const std::vector<cv::Rect> &boxes) {
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.5, 0.4, indices);

    for (int idx : indices) {
        cv::Rect box = boxes[idx];
        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 3);
        std::string label = "";
        if (classIds[idx] < _classNames.size()) {
            label = _classNames[classIds[idx]];
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
    return frame;
}

std::tuple<std::vector<cv::Rect>, std::vector<float>, std::vector<int>> ObjectDetection::detectObjects(const cv::Mat frame) {
    cv::Mat blob;
    std::vector<cv::Mat> outputs;
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    preprocess(frame, blob);
    runModel(blob, outputs);
    postprocess(outputs, frame, classIds, confidences, boxes);

    return std::make_tuple(boxes, confidences, classIds);
}

void ObjectDetection::addLatestFrame(const cv::Mat &frame) {
    std::unique_lock<std::mutex> guard(mutex);
    _latest_frame = frame;
}

std::optional<Detection> ObjectDetection::getLatestDetection (){
    return _latest_detection;
}

void ObjectDetection::run(){
    _running = true;
    _t = std::thread([&](){
        while (_running){
            if (_latest_frame.empty()){
                continue;
            }
            auto [boxes, confidences, classIds] = detectObjects(_latest_frame);
            if (boxes.empty()){
                continue;
            }
            Detection d;
            d.boxes = boxes;
            d.confidences = confidences;
            d.classIds = classIds;
            _latest_detection = d;
        }
    });
}

void ObjectDetection::stop(){
    _running = false;
    _t.join();
}