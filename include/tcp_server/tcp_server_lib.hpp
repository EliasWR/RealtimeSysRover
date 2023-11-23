#ifndef REALTIMESYSROVER_TCP_SERVER2_HPP
#define REALTIMESYSROVER_TCP_SERVER2_HPP

#include "network_helper.hpp"
#include "message_handling/message_handler.hpp"

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
  explicit Connection(tcp::socket socket, std::shared_ptr<MessageHandler> handler);
    ~Connection();
  void run();
  std::string getIPv4();
  void set_message_handler(std::shared_ptr<MessageHandler> handler);
  //using Callback = std::function<void(const std::string &request, std::string &response)>;

  int receiveMessageSize();
  std::string receiveMessage();
  void writeMsg(const std::string &msg);

private:
  tcp::socket _socket;
  std::shared_ptr<MessageHandler> _message_handler;
  std::thread _thread;
};

class TCPServer {
public:
  explicit TCPServer(int port, std::unique_ptr<MessageHandler> handler);
  void start();
  void stop();

  void writeToClient(size_t client_index, const std::string &msg);
  void writeToAllClients(const std::string &msg);

private:
  std::vector<std::unique_ptr<Connection>> _clients;
  std::shared_ptr<MessageHandler> _message_handler;

  asio::io_context _io_context;
  tcp::acceptor _acceptor;
  std::thread _acceptor_thread;

  int _port;
  std::mutex _m;
  std::atomic<bool> _is_running{false};
};


#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
