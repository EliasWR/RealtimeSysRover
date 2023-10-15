//
// Created by Martin on 03.10.2023.
//

#ifndef REALTIMESYSROVER_GUI_COMMUNICATION_HPP
#define REALTIMESYSROVER_GUI_COMMUNICATION_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;

class Session {
private:
  websocket::stream<tcp::socket> ws_;

public:
  explicit Session(tcp::socket socket) : ws_(std::move(socket)) {}

  void start() {
    try {
      ws_.accept();
      for (;;) {
        beast::flat_buffer buffer;
        ws_.read(buffer);
        on_message(buffer);
      }
    } catch (const beast::system_error& se) {
      if (se.code() != websocket::error::closed)
        std::cerr << "Session Error: " << se.code().message() << std::endl;
    } catch (const std::exception& e) {
      std::cerr << "Session Error: " << e.what() << std::endl;
    }
  }

  virtual void on_message(beast::flat_buffer& buffer) {
    std::cout << "Received message: " << beast::make_printable(buffer.data()) << std::endl;
  }
};

class Server {
private:
  net::io_context ioc_;
  tcp::acceptor acceptor_;

public:
  Server(const net::ip::address& address, unsigned short port)
      : ioc_(1), acceptor_(ioc_, {address, port}) {}

  void run() {
    try {
      for (;;) {
        tcp::socket socket{ioc_};
        acceptor_.accept(socket);
        std::jthread([sock = std::move(socket)]() mutable {
          Session session(std::move(sock));
          session.start();
        }).detach();
      }
    } catch (const std::exception& e) {
      std::cerr << "Server Error: " << e.what() << std::endl;
    }
  }
};








#endif // REALTIMESYSROVER_GUI_COMMUNICATION_HPP
