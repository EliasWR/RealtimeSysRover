#include "GUI_communication.hpp"
#include "GUI_helpers.hpp"

#include <boost/beast/websocket.hpp>

#include <iostream>
namespace websocket = boost::beast::websocket;

int main() {
  TCPServer_ server(12345);
  std::string command = "This is command!";
  std::cout << "command: " << command << std::endl;

  server.set_callback([&](Connection& conn, const std::string &msg, std::string &response){
/*
    try {
      websocket::stream<tcp::socket> ws{conn.socket()};
      ws.accept();

      for (;;) {
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "Received message: " << beast::make_printable(buffer.data()) << std::endl;
      }
    }
    catch (beast::system_error const& se) {
      if (se.code() != websocket::error::closed)
        std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch (std::exception const& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
*/

    std::cout << "I am callback!" << std::endl;
    message_handler(msg);
    response = "ok";
  });
  server.start();

  std::cout << "Press a key + 'enter' to continue..." << std::endl;
  while(std::cin.get() != '\n'){}

  server.stop();

  std::cout << "command: " << command << std::endl;

  return 0;
}