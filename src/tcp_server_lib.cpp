#include "tcp_server/tcp_server_lib.hpp"

// Connection Implementation
Connection::Connection(tcp::socket socket) :
    _socket(std::move(socket)) {}


void Connection::start() {
  _is_running = true;
  _thread = std::thread([&] {
    try {
      while (_is_running) {
        auto msg = receiveMessage();
        if (_message_handler) {
            _message_handler(msg);
        }
      }
    } catch (const std::exception &ex) {
      std::cerr << "[TCPConnection Error]: " << ex.what() << std::endl;
      if (_disconnection_handler) {
          _disconnection_handler(this);
      }
    }
  });

}

std::string Connection::getIPv4() {
  return _socket.remote_endpoint().address().to_string();
}

int Connection::receiveMessageSize() {
  std::array<unsigned char, 4> buf{};
  boost::asio::read(_socket, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
  return bytes_to_int(buf);
}

std::string Connection::receiveMessage() {
  int len = receiveMessageSize();
  boost::asio::streambuf buf;
  boost::system::error_code err;
  //asio::read(*_socket, buf, err);

  boost::asio::read(_socket, buf, boost::asio::transfer_exactly(len), err);

  if (err) {
    throw boost::system::system_error(err);
  }
  //std::string data(boost::asio::buffer_cast<const char *>(buf.data()));


  std::string data(boost::asio::buffer_cast<const char *>(buf.data()), len);
  return data;
}

void Connection::writeMessage(const std::string &msg) {
  int msgSize = static_cast<int>(msg.size());

  _socket.send(boost::asio::buffer(int_to_bytes(msgSize), 4));
  _socket.send(boost::asio::buffer(msg));
}

void Connection::stop() {
  {
    std::unique_lock<std::mutex> lock(_m);
    _is_running = false;
  }
  _socket.close();
  _thread.join();
}

void Connection::setDisconnectionHandler(const std::function<void(Connection *)> &handler) {
    _disconnection_handler = handler;
}

void Connection::setMessageHandler(std::function<void(const std::string &)> handler) {
    _message_handler = std::move(handler);
}

// TCPServer Implementation
TCPServer::TCPServer(unsigned short port) :
    _port(port),
    _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
    _is_running(false) {
}

void TCPServer::start() {
  _is_running = true;
  std::cout << "TCP Server running on port: " << _port << std::endl;

  stop_task_thread = false;
  tasks_thread = std::thread(&TCPServer::processTasks, this);

  _acceptor_thread = std::thread([&] {
    try {
      while (_is_running) {

        auto socket = tcp::socket(_io_context);
        _acceptor.accept(socket);

        std::cout << "TCP Connection accepted with: " << socket.remote_endpoint().address().to_string() << std::endl;

        auto connection = std::make_unique<Connection>(std::move(socket));
        _clients.push_back(std::move(connection));
        _clients.back()->setDisconnectionHandler([&](Connection* conn) {
            handleDisconnection(conn);
        });
        _clients.back()->setMessageHandler(_message_handler);
        _clients.back()->start();

      }
    } catch (const boost::system::system_error &e) {
        std::cerr << "[TCPServer] Exception while accepting (Boost system error): " << e.code() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "[TCPServer] Exception while accepting: " << e.what() << std::endl;
    }
  });
}


void TCPServer::stop() {
  if (_is_running) {
      std::cout << "Stopping TCP Server..\n";
      _is_running = false;

      _acceptor.cancel();
      _acceptor.close();
      _acceptor_thread.join();

      std::cout << "Acceptor closed.\n";

      {
        std::lock_guard<std::mutex> lock(_task_m);
        stop_task_thread = true;
      }
      tasks_cond.notify_one();
      if (tasks_thread.joinable()) {
        tasks_thread.join();
      }

      {
        std::unique_lock<std::mutex> lock(_m);
        for (auto &client : _clients) {
          client->stop();
        }
        _clients.clear();
      }

      std::cout << "Clients disconnected.\n";

      _io_context.stop();

      std::cout << "DONE" << std::endl;
  }
}

void TCPServer::writeToClient(size_t client_index, const std::string &msg) {
  if (_clients.size() >= client_index + 1) {
    std::cout << "Writing to client " << client_index << ": " << msg << std::endl;
    _clients[client_index]->writeMessage(msg);
  } else {
    throw std::out_of_range("Client index out of range");
  }
}

void TCPServer::writeToAllClients(const std::string &msg) {
  try {
    if (_clients.empty()) {
      std::cout << "No clients connected." << std::endl;
      return;
    } else {
      for (size_t i = 0; i < _clients.size(); i++) {
          std::cout << "Writing to client " << i << ": " << msg << std::endl;
          _clients[i]->writeMessage(msg);
      }
    }
  }
    catch (const std::exception &e) {
        std::cerr << "Exception while writing to all clients: " << e.what() << std::endl;
    }
}

void TCPServer::handleDisconnection(Connection* conn) {
    {
        std::lock_guard<std::mutex> lock(_task_m);
        tasks.push([this, conn]() {
          auto connection_itr = std::find_if(_clients.begin(), _clients.end(),
                                 [conn](const std::unique_ptr<Connection>& client) {
                                   return client.get() == conn;
                                 });
          if (connection_itr != _clients.end()) {
            std::cout << "Client disconnected: " << (*connection_itr)->getIPv4() << std::endl;
            (*connection_itr)->stop(); // Now safe to call stop()
            _clients.erase(connection_itr);
          }
        });
    }
    tasks_cond.notify_one();
}

void TCPServer::processTasks() {
    while (true) {
        std::function<void()> task;
        {
      std::unique_lock<std::mutex> lock(_task_m);
      tasks_cond.wait(lock, [this] { return stop_task_thread || !tasks.empty(); });
      if (stop_task_thread && tasks.empty()) {
          break;
      }
      task = std::move(tasks.front());
      tasks.pop();
        }
        if (task) {
      task();
        }
    }
}

void TCPServer::setMessageHandler(std::function<void(const std::string &)> handler) {
    _message_handler = std::move(handler);
}
