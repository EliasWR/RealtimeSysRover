#include "tcp_server/ws_server_lib.hpp"

WSConnection::WSConnection(tcp::socket socket) : _socket(std::move(socket)) {
}

void WSConnection::start() {
  try {
    _socket.accept();
    while (true) {
      beast::flat_buffer buffer;
      _socket.read(buffer);
      on_message(buffer);
    }
  } catch (const beast::system_error &se) {
    if (se.code() != websocket::error::closed)
      std::cerr << "[WSConnection Error]: " << se.code().message() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[General WSConnection Error]: " << e.what() << std::endl;
  }
}

void WSConnection::set_callback(Callback callback) {
  _callback = std::move(callback);
}

int WSConnection::receiveMessageSize() {
  beast::flat_buffer buffer;
  _socket.read_some(buffer, 4);
  std::stringstream ss;
  ss << beast::make_printable(buffer.data());
  std::string data = ss.str();
  try {
    return std::stoi(data);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void WSConnection::on_message(beast::flat_buffer &buffer) {
  if (_callback) {
    std::stringstream ss;
    ss << beast::make_printable(buffer.data());
    std::string request = ss.str();
    std::string response;
    _callback(request, response);
    _socket.write(asio::buffer(response));
    //std::cout << "Callback, received message: " << beast::make_printable(buffer.data()) << std::endl;
  } else {
    std::cout << "No callback, received message: " << beast::make_printable(buffer.data()) << std::endl;
  }
}

WSServer::WSServer(unsigned short port)
    : _ioc(1), _acceptor(_ioc, {asio::ip::make_address("0.0.0.0"), port}) {
}

void WSServer::set_callback(WSConnection::Callback callback) {
  _callback = std::move(callback);
}

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
          WSConnection session(std::move(sock));
          if (_callback) {
            session.set_callback(_callback);
          }
          session.start();
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
