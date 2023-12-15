#include "object_detection/object_detection.hpp"

/**
 * @brief Constructor for ObjectDetection class.
 *
 * @details
 * Loads the model and the class names.
 */
ObjectDetection::ObjectDetection() {
  _category_path = "../resources/yolo/coco.names";
  _model_path = "../resources/yolo/yolov3-tiny.weights";
  _config_path = "../resources/yolo/yolov3-tiny.cfg";
  _class_names = loadFileToVector(_category_path);
  _net = cv::dnn::readNet(_model_path, _config_path);
}

/**
 * @brief Preprocesses the frame for the model.
 *
 * @param frame The frame to preprocess.
 * @param blob The blob to store the preprocessed frame in.
 */
void ObjectDetection::preprocess(const cv::Mat &frame, cv::Mat &blob) {
  double scale_factor {1 / 255.0};
  cv::Size size {cv::Size(416, 416)};
  cv::Scalar mean {cv::Scalar(0, 0, 0)};
  bool swapRB {true};
  bool crop {false};
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
  auto &classIds = detection.class_ids;
  auto &frameSize = detection.frame_size;

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
  auto &class_ids = detection.value().class_ids;

  std::vector<int> indices;
  cv::dnn::NMSBoxes(boxes, confidences, 0.3, 0.4, indices);

  for (int idx : indices) {
    cv::Rect box = boxes[idx];
    cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 3);
    std::string label = "";
    if (class_ids[idx] < _class_names.size()) {
      label = _class_names[class_ids[idx]];
    }
    else {
      std::cerr << "Class ID " << class_ids[idx] << " is out of range." << std::endl;
    }
    label += ": " + cv::format("%.2f", confidences[idx]);

    int base_line;
    cv::Size label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &base_line);
    int top = std::max(box.y, label_size.height);
    cv::rectangle(frame, cv::Point(box.x, top - label_size.height),
                  cv::Point(box.x + label_size.width, top + base_line),
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
  std::unique_lock<std::mutex> guard(_mutex);
  _latest_frame = frame;
  _new_frame_available = true;
  _cv.notify_one();
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
  running = true;
  _t = std::thread([&]() {
    while (running) {
      std::unique_lock<std::mutex> guard(_mutex);
      _cv.wait(guard, [this] {
        return _new_frame_available || !running;
      });

      if (!running) break;

      if (_latest_frame.empty()) {
        _new_frame_available = false;
        continue;
      }

      auto detection = detectObjects(_latest_frame);
      _new_frame_available = false;

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
  running = false;
  _t.join();
}