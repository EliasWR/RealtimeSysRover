#include "tcp_server/tcp_server.hpp"
#include "tcp_server/basic_tcp_server.hpp"
#include "GUI_helpers.hpp"

#include <iostream>

void simple_tcp_server(){
  TCPServer server(12345);
  server.set_callback([](const std::string &msg, std::string &response){
    message_handler(msg);
  });
  server.run();
  std::cout << "Press a key + 'enter' to end..." << std::endl;


}

void tcp_server(){
  TCPServer_ server(12345);
  server.set_callback([](const std::string &msg, std::string &response){
    std::cout << "Message received: " << msg << std::endl;
    response = "I got , " + msg + "!\n";
  });

  server.start();


  std::cout << "Press a key + 'enter' to continue..." << std::endl;
  while (std::cin.get() != '\n') {}

  server.stop();
}



int main() {
  tcp_server();

  return 0;

}

