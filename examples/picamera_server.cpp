#include <boost/asio.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace std;

int main() {
  asio::io_context io_context;

  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8000));

  tcp::socket socket(io_context);

  std::cout << "Waiting for connection on port 8000..." << std::endl;
  acceptor.accept(socket);


  while (true) {
    uint32_t image_len;
    asio::read(socket, asio::buffer(&image_len, sizeof(image_len)));

    if (image_len == 0) break;

    vector<uint8_t> image_data(image_len);
    asio::read(socket, asio::buffer(image_data));

    cv::Mat img = cv::imdecode(image_data, cv::IMREAD_COLOR);
    if (img.empty()) {
      cerr << "Failed to decode the received image." << endl;
      continue;
    }

    cout << "Image is " << img.cols << "x" << img.rows << endl;

    cv::imshow("Received Image", img);
    cv::waitKey(1);// Display the image for a short duration. Adjust as necessary.
  }

  return 0;
}