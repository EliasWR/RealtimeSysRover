#ifndef REALTIMESYSROVER_TCP_SERVER2_HPP
#define REALTIMESYSROVER_TCP_SERVER2_HPP

#include "helpers/network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <string>
#include <iostream>
#include <utility>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

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
  void setDisconnectionHandler(const std::function<void(Connection*)>& handler);

private:
  std::function<void(Connection*)> _disconnection_handler;

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
  void processTasks();

private:
  std::queue<std::function<void()>> tasks;
  std::mutex _task_m;
  std::condition_variable tasks_cond;
  std::thread tasks_thread;
  bool stop_task_thread{false};

  std::vector<std::unique_ptr<Connection>> _clients;

  asio::io_context _io_context;
  tcp::acceptor _acceptor;
  std::thread _acceptor_thread;
  Connection::Callback _callback;
  void handleDisconnection(Connection* conn);

  unsigned short _port;
  std::mutex _m;
  std::atomic<bool> _is_running{false};
};


#endif//REALTIMESYSROVER__TCP_SERVER2_HPP
