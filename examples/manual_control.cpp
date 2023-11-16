#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <chrono>

#include "gui_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "tcp_server/ws_tcp_server.hpp"

#include <iostream>

void websocket_server() {
  WSServer server(12345);
  server.set_callback([](const std::string &msg, std::string &response) {
    message_handler(msg);
  });
  server.start();
  std::cout << "Press a key + 'enter' to end..." << std::endl;
}

void tcp_server() {
  TCPServer server(12345);
  server.set_callback([](const std::string &msg, std::string &response) {
    std::cout << "Message received: " << msg << std::endl;
    response = "I got , " + msg + "!\n";
  });

  server.start();

  std::cout << "Press a key + 'enter' to continue..." << std::endl;
  while (std::cin.get() != '\n') {}

  server.stop();
}



int main() {
  SafeQueue<std::string> command_queue;
  std::atomic<bool> stop{false};

  auto WebsocketServer = WSServer(12345);
  WebsocketServer.set_callback([&](const std::string &msg, std::string &response) {
    auto command = message_handler(msg);
    command_queue.enqueue(command);
  });
  WebsocketServer.start();

  auto TCP = TCPServer(9091);
  TCP.start();



  auto internal_comm_thread = std::thread([&] {
    auto last_msg_time = std::chrono::steady_clock::now();
    while (!stop){
      auto now = std::chrono::steady_clock::now();
      auto cmd = command_queue.dequeue();
      if (cmd.has_value()) {
        json j = json::parse(cmd.value());

        if (j["command"] == "stop") {
          last_msg_time = now;
          TCP.writeToAllClients(cmd.value());
        } else if (now - last_msg_time > std::chrono::milliseconds(10)) {
          last_msg_time = now;
          TCP.writeToAllClients(cmd.value());
        }
      }
    }
  });

  std::cout << "Press a key + 'enter' to end..." << std::endl;
  while (std::cin.get() != '\n') {
  }
  std::cout << "Stopping..." << std::endl;
  stop = true;

  WebsocketServer.stop();
  TCP.stop();

  std::cout << "Joining..." << std::endl;
  internal_comm_thread.join();

  std::cout << "Joined" << std::endl;


  return 0;
}
