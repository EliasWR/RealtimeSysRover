#include "tcp_server/tcp_server_lib.hpp"

int main(){
  auto server = TCPServer(9095);
  server.set_callback([](const std::string &msg, std::string &response) {
    std::cout << "Message received: " << msg << std::endl;
    response = "I got , " + msg + "!\n";
  });


  server.start();

    std::cout << "Press a key + 'enter' to continue..." << std::endl;
    while (std::cin.get() != '\n') {}

    server.stop();

  return 0;
}