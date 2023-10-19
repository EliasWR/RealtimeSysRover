#include "GUI_communication.hpp"
#include "GUI_helpers.hpp"


int main() {
  TCPServer server(asio::ip::make_address("0.0.0.0"), 12345);
  server.run();
  return 0;
}