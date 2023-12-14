#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>

#include "nlohmann/json.hpp"

#include "helpers/gui_helper.hpp"
#include "helpers/network_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "tcp_server/ws_server_lib.hpp"
#include "udp_server/udp_server.hpp"
#include "video_viewer/video_viewer.hpp"

using json = nlohmann::json;

int main() {
  SafeQueue<std::string> command_queue;
  std::atomic<bool> stop_comm_thread{false};

  auto Viewer = std::make_unique<VideoViewer>("Camera View");

  auto WebsocketServer{WSServer(12345)};
  WebsocketServer.setMessageHandler([&](const std::string &msg) {
    auto command = GUI::message_handler(msg);
    command_queue.enqueue(command);
  });
  WebsocketServer.start();

  auto TCPServer = TCP::TCPServer(9091);
  TCPServer.start();

  auto internal_comm_thread = std::thread([&] {
    auto last_msg_time = std::chrono::steady_clock::now();
    while (!stop_comm_thread) {
      auto now = std::chrono::steady_clock::now();
      auto cmd = command_queue.dequeue();
      if (cmd.has_value()) {
        json j = json::parse(cmd.value());

        if (j["command"] == "stop" or j["command"] == "reset_camera") {
          TCPServer.writeToAllClients(cmd.value());
        }
        else if (now - last_msg_time > std::chrono::milliseconds(200)) {
          last_msg_time = now;
          TCPServer.writeToAllClients(cmd.value());
        }
      }
    }
  });

  auto handler_proto = [&](const std::string &message) {
    cv::Mat decoded_frame = decodeImageFromProto(message);
    Viewer->addFrame(decoded_frame);
  };

  auto udp_server{UDPServer(8080, handler_proto)};

  udp_server.start();

  auto fps = 30;
  auto frame_interval = std::chrono::milliseconds(1000 / fps);

  std::cout << "Press any key while in Camera View to end..." << std::endl;
  while (true) {
    Viewer->display();
    if (cv::waitKey(frame_interval.count()) >= 0) break;
  }

  /* Cleanup */
  stop_comm_thread = true;

  WebsocketServer.stop();
  TCPServer.stop();
  udp_server.stop();

  internal_comm_thread.join();


  return 0;
}