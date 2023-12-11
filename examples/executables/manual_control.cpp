#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <chrono>

#include "helpers/gui_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "tcp_server/ws_server_lib.hpp"

#include "nlohmann/json.hpp"
#include <iostream>

using json = nlohmann::json;

int main() {
  SafeQueue<std::string> command_queue;
  std::atomic<bool> stop_comm_thread{false};

  auto WebsocketServer{WSServer(12345)};
  WebsocketServer.setMessageHandler([&](const std::string &msg) {
    /* Parses a message from the GUI and enqueues a JSON command to be sent to the rover */
    auto command = GUI::message_handler(msg);
    command_queue.enqueue(command);
  });
  WebsocketServer.start();

  auto TCPServer{TCP::TCPServer(9091)};
  TCPServer.start();
  TCPServer.setMessageHandler([&](const std::string &msg) {
    /* TCP messages from the rover are not used for anything yet.*/
    std::cout << "TCP Message: " << msg << std::endl;
  });

  auto internal_comm_thread = std::thread([&] {
    auto last_msg_time = std::chrono::steady_clock::now();
    while (!stop_comm_thread) {
      auto now = std::chrono::steady_clock::now();
      auto cmd = command_queue.dequeue();

      if (cmd.has_value()) {
        json j = json::parse(cmd.value());

        /* Only send a command to the rover every 100ms, except stop or reset-camera.*/
        if (j["command"] == "stop" or j["command"] == "reset_camera") {
          TCPServer.writeToAllClients(cmd.value());
        }
        else if (now - last_msg_time > std::chrono::milliseconds(100)) {
          last_msg_time = now;
          TCPServer.writeToAllClients(cmd.value());
        }
      }
    }
  });

  std::cout << "Press any key + 'enter' to end..." << std::endl;
  while (std::cin.get() != '\n') {}

  /* Cleanup */
  stop_comm_thread = true;

  WebsocketServer.stop();
  TCPServer.stop();

  internal_comm_thread.join();

  return 0;
}
