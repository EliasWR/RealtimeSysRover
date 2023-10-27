#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>

namespace asio = boost::asio;
using asio::ip::udp;
using namespace std;

int main() {
  asio::io_context io_context;

  udp::socket socket(io_context, udp::endpoint(udp::v4(), 8000));

  std::cout << "Listening for data on port 8000..." << std::endl;

  while (true) {
    udp::endpoint sender_endpoint;  // To store client information
        
    uint32_t image_len;
    socket.receive_from(asio::buffer(&image_len, sizeof(image_len)), sender_endpoint);

    if (image_len == 0) break;

    vector<uint8_t> image_data(image_len);
    socket.receive_from(asio::buffer(image_data), sender_endpoint);

    cv::Mat img = cv::imdecode(image_data, cv::IMREAD_COLOR);
    if (img.empty()) {
      cerr << "Failed to decode the received image." << endl;
      continue;
    }

    cout << "Image is " << img.cols << "x" << img.rows << endl;

    cv::imshow("Received Image", img);
    cv::waitKey(1);  // Display the image for a short duration. Adjust as necessary.
  }

  return 0;
}