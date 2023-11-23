#include "video_viewer/video_viewer.hpp"

VideoViewer::VideoViewer():
    dummy_frame(cv::Mat::zeros(cv::Size(640, 480), CV_8UC3)){
    cv::putText(dummy_frame, "Waiting for video feed...", cv::Point(50, 240),
              cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);
}

VideoViewer::~VideoViewer(){
  // Signal the display loop to stop and join the thread
  cv::destroyWindow("Video Viewer");
}

void VideoViewer::addFrame(const cv::Mat &frame) {
  std::unique_lock<std::mutex> guard(queue_mutex);
  if (!frame.empty()) {
    frame_queue.push(frame);
  }
}

void VideoViewer::display() {
  cv::Mat frame{getFrameFromQueue()};
  cv::imshow("Video Viewer", frame);
  cv::waitKey(10); // Adjust delay as needed
}

cv::Mat VideoViewer::getFrameFromQueue() {
  std::unique_lock<std::mutex> guard(queue_mutex);
  cv::Mat frame;
  if (!frame_queue.empty()) {
    frame = frame_queue.front();
    if(frame_queue.size() > 1){
      frame_queue.pop();
    }
  } else {
    frame = dummy_frame;
  }
  return frame;
}