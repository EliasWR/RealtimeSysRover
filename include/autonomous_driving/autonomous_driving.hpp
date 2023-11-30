#ifndef REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP
#define REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP

#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "helpers/detection_helper.hpp"

class AutonomousDriving {
public:
    AutonomousDriving();
    void run ();
    void stop ();
    void addLatestDetection (const cv::Mat& frame);
    void getLatestCommand (std::string& command);
    bool _running{false};
private:
    std::thread _t;
    cv::Mat _latest_frame;
    std::mutex mutex;
    Detection _latest_detection;
};



#endif //REALTIMESYSROVER_AUTONOMOUS_DRIVING_HPP