//
// Created by Martin on 03.10.2023.
//

#include "test_GUI.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <thread>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;

void do_session(tcp::socket socket) {
  try {
    websocket::stream<tcp::socket> ws{std::move(socket)};
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
}

int main(int argc, char* argv[]) {
  try {
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(12345);

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    for (;;) {
      tcp::socket socket{ioc};
      acceptor.accept(socket);
      std::jthread([sock = std::move(socket)]() mutable {
        do_session(std::move(sock));
      }).detach();
    }
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}