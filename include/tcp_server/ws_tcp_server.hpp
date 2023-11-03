#include "network_helper.hpp"

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace websocket = beast::websocket;

using tcp = boost::asio::ip::tcp;

class WSConnection {
private:
  websocket::stream<tcp::socket> _socket;

public:
  using Callback = std::function<void(const std::string &request, std::string &response)>;

  explicit WSConnection(tcp::socket socket) :
      _socket(std::move(socket)) {
  }

  void start() {


    try {
      _socket.accept();
      while (true) {
        beast::flat_buffer buffer;
        _socket.read(buffer);
        on_message(buffer);
      }
    } catch (const beast::system_error &se) {
      if (se.code() != websocket::error::closed)
        std::cerr << "Session Error: " << se.code().message() << std::endl;
    } catch (const std::exception &e) {
      std::cerr << "Session Error: " << e.what() << std::endl;
    }
  }

  void set_callback(Callback callback) {
    _callback = std::move(callback);
  }

  int receiveMessageSize() {
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

  virtual void on_message(beast::flat_buffer &buffer) {
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

private:
  Callback _callback;
};

class WSServer {
private:
  asio::io_context _ioc;
  tcp::acceptor _acceptor;
  std::jthread _acceptor_thread;
  WSConnection::Callback _callback;
  std::atomic<bool> _is_running{false};

public:
  explicit WSServer(unsigned short port) :
      _ioc(1), _acceptor(_ioc, {asio::ip::make_address("0.0.0.0"), port}) {
  }

  void set_callback(WSConnection::Callback callback) {
    _callback = std::move(callback);
  }

  void start() {
    _is_running = true;
    std::cout << "Serving Websocket connections on port " << _acceptor.local_endpoint().port() << std::endl;

    _acceptor_thread = std::jthread([&] {
      try {
        while (_is_running) {

          tcp::socket socket{_ioc};
          _acceptor.accept(socket);//blocking operation

          std::cout << "Websocket connected to client at " << socket.remote_endpoint().address().to_string() << std::endl;

          std::jthread([&, sock = std::move(socket)]() mutable {
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

    void stop() {
        if (_is_running) {
        std::cout << "Stopping Websocket Server..";
        _is_running = false;
        _acceptor.close();
        std::cout << "DONE" << std::endl;
        }
    }
};

