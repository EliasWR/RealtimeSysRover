#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "gui_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server_lib.hpp"
#include "tcp_server/ws_tcp_server.hpp"
#include "udp_server/udp_server.hpp"

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
  TCPServer_ server(12345);
  server.set_callback([](const std::string &msg, std::string &response) {
    std::cout << "Message received: " << msg << std::endl;
    response = "I got , " + msg + "!\n";
  });

  server.start();

  std::cout << "Press a key + 'enter' to continue..." << std::endl;
  while (std::cin.get() != '\n') {}

  server.stop();
}

void udp_server () {
    const int port = 8080;
    UDPServer server(port);
    server.start();
}

int main() {
  SafeQueue<std::string> command_queue;

  auto WebsocketServer = WSServer(12345);
  WebsocketServer.set_callback([&](const std::string &msg, std::string &response) {
    auto command = message_handler(msg);
    command_queue.enqueue(command);
  });
  WebsocketServer.start();

  auto TCPServer = TCPServer_(9091);
  TCPServer.start();

  udp_server();

  auto internal_comm_thread = std::thread([&] {
    while (true){
      auto cmd = command_queue.dequeue();
      if (cmd.has_value()){
        std::cout << cmd.value() << std::endl;
        TCPServer.writeToAllClients(cmd.value());
      } else {
        break;
      }
    }
  });

  std::cout << "Press a key + 'enter' to end..." << std::endl;
  while (std::cin.get() != '\n') {
  }
  std::cout << "Stopping..." << std::endl;

  WebsocketServer.stop();
  TCPServer.stop();
  command_queue.stop();

  std::cout << "Joining..." << std::endl;
  internal_comm_thread.join();

  std::cout << "Joined" << std::endl;


  return 0;
}
