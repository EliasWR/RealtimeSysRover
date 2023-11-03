#include "gui_helper.hpp"
#include "safe_queue/safe_queue.hpp"
#include "tcp_server/tcp_server.hpp"
#include "tcp_server/ws_tcp_server.hpp"

#include <iostream>

void websocket_server() {
  WSServer server(12345);
  server.set_callback([](const std::string &msg, std::string &response) {
    message_handler(msg);
  });
  server.run();
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
  while (std::cin.get() != '\n') {
  }

  server.stop();
}

// Function to handle commands from the GUI and add them to the queue
void guiCommandHandler(const std::string &msg) {
  // Parse and validate the command from the GUI
  // If it's a command for the Raspberry Pi, enqueue it
}

int main() {
  SafeQueue<std::string> commandQueue;

  auto WebsocketServer = WSServer(12345);
  WebsocketServer.set_callback([ & ](const std::string &msg, std::string &response) {
    auto command = message_handler(msg);
    commandQueue.enqueue(command);
  });
  WebsocketServer.run();

  auto TCPServer = TCPServer_(9091);
  TCPServer.start();

  auto j = std::thread([ & ] {
    while (true) {
      std::string cmd = commandQueue.dequeue();
      std::cout << cmd << std::endl;
      TCPServer.writeToAllClients(cmd);
    }
  });

  std::cout << "Press a key + 'enter' to end..." << std::endl;
  while (std::cin.get() != '\n') {
  }
  j.join();
  TCPServer.stop();

  return 0;
}
