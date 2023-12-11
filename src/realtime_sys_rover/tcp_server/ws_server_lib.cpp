#include "tcp_server/ws_server_lib.hpp"


/***********************************************************************************************************************
 *  Websocket-Connection
 */

/*
   Constructor for establishing a Websocket connection.

   @param socket: The Websocket socket used for the connection.
 */
WSConnection::WSConnection(tcp::socket socket) :
    _socket(std::move(socket)) {
}

/*
 * Starts the connection-loop
 *
 * Creates a thread that runs a loop that receives messages and calls the message handler.
 */
void WSConnection::start() {
  try {
    _socket.accept();
    while (true) {
      auto msg = receiveMessage();
      if (_message_handler) {
        _message_handler(msg);
      }
    }
  } catch (const beast::system_error &se) {
    if (se.code() != websocket::error::closed)
      std::cerr << "[WSConnection Error]: " << se.code().message() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[General WSConnection Error]: " << e.what() << std::endl;
  }
}

/*
 * Receives the size of the incoming message.
 *
 * @return int: The size of the incoming message.
 */
std::string WSConnection::receiveMessage() {
  beast::flat_buffer buffer;
  _socket.read(buffer);
  std::stringstream ss;
  ss << beast::make_printable(buffer.data());
  std::string data = ss.str();
  return data;
}

/*
 * Sets the message handler.
 *
 * @param handler: The message handler.
 */
void WSConnection::setMessageHandler(std::function<void(const std::string &)> handler) {
  _message_handler = std::move(handler);
}


/***********************************************************************************************************************
 *  Websocket-Server
 */

/*
   Constructor for establishing a Websocket server.

   @param port: The port on which the server should listen.
 */
WSServer::WSServer(unsigned short port) :
    _ioc(1), _acceptor(_ioc, {asio::ip::make_address("0.0.0.0"), port}) {
}

/*
 * Starts the server.
 *
 * Creates a thread that runs a loop that accepts incoming connections and creates a new thread for each connection.
 */
void WSServer::start() {
  _is_running = true;
  std::cout << "Serving Websocket connections on port " << _acceptor.local_endpoint().port() << std::endl;

  _acceptor_thread = std::thread([&] {
    try {
      while (_is_running) {

        tcp::socket socket{_ioc};
        _acceptor.accept(socket);//blocking operation

        std::cout << "Websocket connected to client at " << socket.remote_endpoint().address().to_string() << std::endl;

        std::thread([&, sock = std::move(socket)]() mutable {
          WSConnection connection(std::move(sock));
          if (_message_handler) {
            connection.setMessageHandler(_message_handler);
          }
          connection.start();
        }).detach();
      }
    } catch (const beast::system_error &e) {
      if (e.code() != websocket::error::closed) {
        std::cerr << "Boost System Error: " << e.code() << std::endl;
      }
    } catch (const std::exception &e) {
      std::cerr << "Server Error: " << e.what() << std::endl;
    }
  });
}

/*
 * Stops the server.
 *
 * Stops the acceptor thread and the io_context.
 */
void WSServer::stop() {
  if (_is_running) {
    std::cout << "Stopping Websocket Server..\n";
    _is_running = false;
    _acceptor.close();
    _acceptor_thread.join();
    _ioc.stop();
    std::cout << "DONE" << std::endl;
  }
}

/*
 * Sets the message handler.
 *
 * @param handler: The message handler.
 */
void WSServer::setMessageHandler(std::function<void(const std::string &)> handler) {
  _message_handler = std::move(handler);
}
