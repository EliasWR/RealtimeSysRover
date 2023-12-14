#include "object_detection/object_detection.hpp"

/**
 * @brief Constructor for ObjectDetection class.
 *
 * @details
 * Loads the model and the class names.
 */
ObjectDetection::ObjectDetection() {
  _categoryPath = "../../resources/yolo/coco.names";
  _modelPath = "../../resources/yolo/yolov3-tiny.weights";
  _configPath = "../../resources/yolo/yolov3-tiny.cfg";
  _classNames = loadFileToVector(_categoryPath);
  _net = cv::dnn::readNet(_modelPath, _configPath);
}

/**
 * @brief Preprocesses the frame for the model.
 *
 * @param frame The frame to preprocess.
 * @param blob The blob to store the preprocessed frame in.
 */
void ObjectDetection::preprocess(const cv::Mat &frame, cv::Mat &blob) {
  double scale_factor = 1 / 255.0;
  cv::Size size = cv::Size(416, 416);
  cv::Scalar mean = cv::Scalar(0, 0, 0);
  bool swapRB = true;
  bool crop = false;
  cv::dnn::blobFromImage(frame, blob, scale_factor, size, mean, swapRB, crop);
}

/**
 * @brief Runs the model.
 *
 * @details
 * The model is run on the blob and the outputs are stored in the outputs vector.
 *
 * @param blob The blob to run the model on.
 * @param outputs The outputs of the model.
 */
void ObjectDetection::runModel(const cv::Mat &blob, std::vector<cv::Mat> &outputs) {
  _net.setInput(blob);
  _net.forward(outputs, _net.getUnconnectedOutLayersNames());
}

/**
 * @brief Postprocesses the outputs of the model.
 *
 * @details
 * The outputs are postprocessed and stored in the detection object.
 *
 * @param outputs The outputs of the model.
 * @param frame The frame to postprocess.
 * @param detection The detection object to store the postprocessed outputs in.
 */
void ObjectDetection::postprocess(const std::vector<cv::Mat> &outputs, const cv::Mat &frame, Detection &detection) {
  auto &boxes = detection.boxes;
  auto &confidences = detection.confidences;
  auto &classIds = detection.classIds;
  auto &frameSize = detection.frameSize;

  double detection_threshold = 0.3;
  for (auto &output : outputs) {
    auto *data = reinterpret_cast<float *>(output.data);
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
        frameSize = std::make_pair(frame.cols, frame.rows);
      }
    }
  }
}

/**
 * @brief Draws the detections on the frame.
 *
 * @param frame The frame to draw the detections on.
 * @param detection The detection object containing the detections.
 * @return The frame with the detections drawn on it.
 */
cv::Mat ObjectDetection::drawDetections(cv::Mat &frame, std::optional<Detection> &detection) {
  if (detection == std::nullopt) {
    return frame;
  }

  auto &boxes = detection.value().boxes;
  auto &confidences = detection.value().confidences;
  auto &classIds = detection.value().classIds;

  std::vector<int> indices;
  cv::dnn::NMSBoxes(boxes, confidences, 0.3, 0.4, indices);

  for (int idx : indices) {
    cv::Rect box = boxes[idx];
    cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 3);
    std::string label = "";
    if (classIds[idx] < _classNames.size()) {
      label = _classNames[classIds[idx]];
    }
    else {
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

/**
 * @brief Detects objects in the frame.
 *
 * @param frame The frame to detect objects in.
 * @return The detection object containing the detections.
 */
Detection ObjectDetection::detectObjects(const cv::Mat &frame) {
  cv::Mat blob;
  std::vector<cv::Mat> outputs;
  Detection detection;

  preprocess(frame, blob);
  runModel(blob, outputs);
  postprocess(outputs, frame, detection);

  return detection;
}

/**
 * @brief Adds the latest frame to the object detection.
 *
 * @param frame The latest frame.
 */
void ObjectDetection::addLatestFrame(const cv::Mat &frame) {
  std::unique_lock<std::mutex> guard(mutex);
  _latest_frame = frame;
  new_frame_available = true;
  cv.notify_one();
}

/**
 * @brief Get the latest detection.
 *
 * @return The latest detection.
 */
std::optional<Detection> ObjectDetection::getLatestDetection() {
  if (std::chrono::steady_clock::now() - _last_detection_time >= std::chrono::milliseconds(MAX_DETECTION_AGE)) {
    return std::nullopt;
  }
  return _latest_detection;
}

/**
 * @brief Runs the object detection.
 *
 * @details
 * Runs the object detection in a separate thread.
 * The latest detection is stored in the _latest_detection variable.
 */
void ObjectDetection::run() {
  _running = true;
  _t = std::thread([&]() {
    while (_running) {
      std::unique_lock<std::mutex> guard(mutex);
      cv.wait(guard, [this] {
        return new_frame_available || !_running;
      });

      if (!_running) break;

      if (_latest_frame.empty()) {
        new_frame_available = false;
        continue;
      }

      auto detection = detectObjects(_latest_frame);
      new_frame_available = false;

      if (!detection.boxes.empty()) {
        _last_detection_time = std::chrono::steady_clock::now();
        _latest_detection = detection;
      }
    }
  });
}

/**
 * @brief Stops the object detection.
 *
 * @details
 * Stops the object detection thread.
 */
void ObjectDetection::stop() {
  _running = false;
  _t.join();
}