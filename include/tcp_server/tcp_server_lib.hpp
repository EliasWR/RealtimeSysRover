#ifndef REALTIMESYSROVER_TCP_SERVER2_HPP
#define REALTIMESYSROVER_TCP_SERVER2_HPP

#include "helpers/network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <string>
#include <iostream>
#include <utility>

namespace beast = boost::beast;
namespace asio = boost::asio;

using tcp = boost::asio::ip::tcp;

class Connection {
public:
  explicit Connection(tcp::socket socket);
  void stop();
  void start();
  std::string getIPv4();
  //using Callback = std::function<void(Connection& conn, const std::string& request, std::string& response)>;
  using Callback = std::function<void(const std::string &request, std::string &response)>;

  void setCallback(Callback &callback);
  int receiveMessageSize();
  std::string receiveMessage();
  void writeMessage(const std::string &msg);

private:
  tcp::socket _socket;
  std::mutex _m;
  Callback _callback;
  std::thread _thread;
  std::atomic<bool> _is_running{false};
};

class TCPServer {
public:
  explicit TCPServer(unsigned short port);
  void start();
  void stop();

  virtual void set_callback(Connection::Callback callback);
  void writeToClient(size_t client_index, const std::string &msg);
  void writeToAllClients(const std::string &msg);

private:
  std::vector<std::unique_ptr<Connection>> _clients;

  asio::io_context _io_context;
  tcp::acceptor _acceptor;
  std::thread _acceptor_thread;
  Connection::Callback _callback;

  unsigned short _port;
  std::mutex _m;
  std::atomic<bool> _is_running{false};
};


#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
