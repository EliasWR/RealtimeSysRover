#include "tcp_server/tcp_server_lib.hpp"


/***********************************************************************************************************************
 *  TCP-Connection
 */

/*
   Constructor for establishing a TCP connection.

   @param socket: The TCP socket used for the connection.
*/
TCP::Connection::Connection(tcp::socket socket) :
    _socket(std::move(socket)) {
}

/*
Starts the connection-loop

Creates a thread that runs a loop that receives messages and calls the message handler.
*/
void TCP::Connection::start() {
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

/*
Returns the IPv4 address of the connection.

@return std::string: The IPv4 address of the connection.
*/
std::string TCP::Connection::getIPv4() {
  return _socket.remote_endpoint().address().to_string();
}

/*
 Receives the size of the incoming message.

 @return int: The size of the incoming message.
*/
int TCP::Connection::receiveMessageSize() {
  std::array<unsigned char, 4> buf{};
  boost::asio::read(_socket, boost::asio::buffer(buf), boost::asio::transfer_exactly(4));
  return bytes_to_int(buf);
}

/*
 Receives a message from the connection.

 First receives the size of the message, then the message itself.
 Throws a boost::system::system_error if an error occurs.

 @return std::string: The received message.
*/
std::string TCP::Connection::receiveMessage() {
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

/*
 Sends a message over the connection.

 First sends the size of the message, then the message itself.

 @param msg: The message to be sent.
*/
void TCP::Connection::writeMessage(const std::string &msg) {
  int msgSize = static_cast<int>(msg.size());

  _socket.send(boost::asio::buffer(int_to_bytes(msgSize), 4));
  _socket.send(boost::asio::buffer(msg));
}

/*
   Stops the connection.

   Closes the socket and joins the thread.
*/
void TCP::Connection::stop() {
  {
    std::unique_lock<std::mutex> lock(_m);
    _is_running = false;
  }
  _socket.close();
  _thread.join();
}

/*
 Sets the disconnection handler.

 @param handler: The disconnection handler.
*/
void TCP::Connection::setDisconnectionHandler(const std::function<void(Connection *)> &handler) {
  _disconnection_handler = handler;
}

/*
 Sets the message handler.

 @param handler: The message handler.
*/
void TCP::Connection::setMessageHandler(std::function<void(const std::string &)> handler) {
  _message_handler = std::move(handler);
}


/***********************************************************************************************************************
 *  TCP-Server
 */

/*
 Constructor for the TCP server.

 Creates the acceptor and binds it to the given port.

 @param port: The port to be used for the server.
*/
TCP::TCPServer::TCPServer(unsigned short port) :
    _port(port),
    _acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
    _is_running(false) {
}

/*
 Starts the TCP server.

 Starts the acceptor thread and the task thread.
*/
void TCP::TCPServer::start() {
  _is_running = true;
  std::cout << "TCP Server running on port: " << _port << std::endl;

  stop_task_thread = false;
  tasks_thread = std::thread(&TCP::TCPServer::processTasks, this);

  _acceptor_thread = std::thread([&] {
    try {
      while (_is_running) {

        auto socket = tcp::socket(_io_context);
        _acceptor.accept(socket);

        std::cout << "TCP Connection accepted with: " << socket.remote_endpoint().address().to_string() << std::endl;

        auto connection = std::make_unique<Connection>(std::move(socket));
        _clients.push_back(std::move(connection));
        _clients.back()->setDisconnectionHandler([&](Connection *conn) {
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

/*
 Stops the TCP server.

 Stops the acceptor and the task thread.
*/
void TCP::TCPServer::stop() {
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

/*
 Writes a message to a client.

 Throws a std::out_of_range exception if the client index is out of range.

 @param client_index: The index of the client to write to.
 @param msg: The message to be written.
*/
void TCP::TCPServer::writeToClient(size_t client_index, const std::string &msg) {
  if (_clients.size() >= client_index + 1) {
    std::cout << "Writing to client " << client_index << ": " << msg << std::endl;
    _clients[client_index]->writeMessage(msg);
  }
  else {
    throw std::out_of_range("Client index out of range");
  }
}

/*
 Writes a message to all clients.

 @param msg: The message to be written.
*/
void TCP::TCPServer::writeToAllClients(const std::string &msg) {
  try {
    if (_clients.empty()) {
      std::cout << "No clients connected." << std::endl;
      return;
    }
    else {
      for (size_t i = 0; i < _clients.size(); i++) {
        std::cout << "Writing to client " << i << ": " << msg << std::endl;
        _clients[i]->writeMessage(msg);
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Exception while writing to all clients: " << e.what() << std::endl;
  }
}

/*
 Handles the disconnection of a client.

 Removes the client from the list of clients.

 @param conn: The connection of the client that disconnected.
*/
void TCP::TCPServer::handleDisconnection(Connection *conn) {
  {
    std::lock_guard<std::mutex> lock(_task_m);
    tasks.push([this, conn]() {
      auto connection_itr = std::find_if(_clients.begin(), _clients.end(),
                                         [conn](const std::unique_ptr<Connection> &client) {
                                           return client.get() == conn;
                                         });
      if (connection_itr != _clients.end()) {
        std::cout << "Client disconnected: " << (*connection_itr)->getIPv4() << std::endl;
        (*connection_itr)->stop();// Now safe to call stop()
        _clients.erase(connection_itr);
      }
    });
  }
  tasks_cond.notify_one();
}

/*
 Processes the tasks.

 Runs a loop that processes the tasks in the task queue.
 For now, the only task is to remove a client from the list of clients.
*/
void TCP::TCPServer::processTasks() {
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(_task_m);
      tasks_cond.wait(lock, [this] {
        return stop_task_thread || !tasks.empty();
      });
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

/*
 Sets the message handler.

 @param handler: The message handler.
*/
void TCP::TCPServer::setMessageHandler(std::function<void(const std::string &)> handler) {
  _message_handler = std::move(handler);
}
