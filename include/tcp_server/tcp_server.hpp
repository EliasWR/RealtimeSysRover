#ifndef REALTIMESYSROVER_TCP_SERVER_HPP
#define REALTIMESYSROVER_TCP_SERVER_HPP

#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>

#include "network_helper.hpp"

namespace beast = boost::beast;
namespace asio = boost::asio;

using tcp = boost::asio::ip::tcp;
using udp = boost::asio::ip::udp;

class Connection {
private:
  beast::websocket::stream<tcp::socket> _socket;
  std::function<std::string(beast::flat_buffer &)> _handler;
  std::jthread _thread;

  int _recieveSize();


public:
  explicit Connection(asio::io_context &);
  explicit Connection(asio::io_context &, const std::function<std::string()>&);
  ~Connection();
  void start();
  void write(const std::string &msg);
  std::string recieveMessage();
};


class TCPServer {
private:
  int _port;
  asio::io_context _ioc;
  tcp::acceptor _acceptor;

  std::function<std::string()> _handler;
  std::jthread _thread;
  std::atomic<bool> _stop_all{false};

public:
  TCPServer(int port);
  void start();
  int stop();
};

#endif// SESSION_HPP


#endif//REALTIMESYSROVER_TCP_SERVER_HPP
