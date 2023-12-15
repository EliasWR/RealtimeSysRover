#include "video_viewer/video_viewer.hpp"

#include <thread>

int main() {
  VideoViewer viewer;

  std::atomic<bool> stop_flag(false);

  std::thread frame_grabber([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    while (!stop_flag) {
      cv::Mat frame = cv::Mat(480, 640, CV_8UC3, cv::Scalar(100, 100, 255));
      viewer.addFrame(frame);
    }
  });

  while (true) {
    viewer.display();

    if (cv::waitKey(10) >= 0) break;
  }
  stop_flag = true;
  frame_grabber.join();
  return 0;
}
