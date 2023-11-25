#include "video_viewer/video_viewer.hpp"


int main() {
  VideoViewer viewer;
  std::atomic<bool> stop_flag(false);

  std::thread frame_grabber([&](){

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    while (!stop_flag) {
      cv::Mat frame = cv::Mat(480, 640, CV_8UC3, cv::Scalar(100, 100, 255)); // Obtain your frame from somewhere (e.g., video capture)
      viewer.addFrame(frame);

    }
  });

  // Example: Loop to add frames
  while (true) {

    viewer.display();
    // ... other code ...

    if (cv::waitKey(10) >= 0) break; // Break the loop on any key press

  }
  stop_flag = true;
  frame_grabber.join();
  return 0;
}
