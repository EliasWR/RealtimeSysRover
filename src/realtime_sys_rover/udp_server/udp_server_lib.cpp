#include "udp_server/udp_server.hpp"

using udp = boost::asio::ip::udp;
namespace asio = boost::asio;

/**
 * @brief Constructor for the UDP server.
 *
 * Initializes the UDP server with a specified port and a handler function for incoming messages.
 * Sets up the UDP socket to listen on the given port.
 *
 * @param port The port number on which the server will listen.
 * @param handler The function to handle incoming messages.
 */
UDPServer::UDPServer(int port, std::function<void(const std::string &)> handler) :
    _port(port), _socket(_io_context, udp::endpoint(udp::v4(), _port)), _messageHandler(std::move(handler)) {
}

/**
 * @brief Starts the UDP server.
 *
 * Begins listening for incoming UDP packets. For each received message, the message handler is invoked.
 * The server runs on a separate thread to handle incoming messages continuously.
 */
void UDPServer::start() {
  std::cout << "Server is listening on port " << _port << std::endl;

  _thread = std::thread([&] {
    try {
      while (true) {
        auto [message, len, endpoint] = receiveMessage();
        _messageHandler(message);
        //standardResponse(endpoint);
      }
    } catch (const std::exception &e) {
      std::cerr << "Exception caught in thread: " << e.what() << '\n';
    } catch (...) {
      std::cerr << "Unknown exception caught in thread.\n";
    }
  });
}

/**
 * @brief Receives a message from a client.
 *
 * @return std::tuple<std::string, size_t, udp::endpoint>: The received message, the length of the message and the
 * endpoint of the client.
 */
std::tuple<std::string, size_t, udp::endpoint> UDPServer::receiveMessage() {
  udp::endpoint remote_endpoint;

  std::vector<char> recv_buffer(65507);
  size_t len = _socket.receive_from(asio::buffer(recv_buffer), remote_endpoint);
  std::string recv_message(recv_buffer.begin(), recv_buffer.begin() + len);
  return std::make_tuple(recv_message, len, remote_endpoint);
}


/**
 * @brief Receives the size of the incoming message.
 *
 * [Optional] Receives the size of the next incoming UDP message. This is useful when expecting
 * fixed-size messages or when the message size needs to be known in advance.
 *
 * @return The size of the incoming message, or an error if the size cannot be determined.
 */
[[maybe_unused]] int UDPServer::receiveMessageSize() {
    std::vector<char> recv_buffer(4);
    size_t len = _socket.receive(asio::buffer(recv_buffer));
    std::string data(recv_buffer.begin(), recv_buffer.begin() + len);
    try {
        return std::stoi(data);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


/**
 * @brief Sends a message to a client.
 *
 * @param message The message to send.
 * @param remote_endpoint The endpoint of the client.
 */
[[maybe_unused]] void UDPServer::sendMessage(const std::string& message, const udp::endpoint& remote_endpoint) {
    _socket.send_to(asio::buffer(message), remote_endpoint);
}

/**
 * @brief Stops the UDP server.
 *
 * Closes the server's socket, stops the IO context, and joins the main server thread.
 * This method ensures a clean shutdown of the server.
 */
void UDPServer::stop() {
  _socket.close();
  _io_context.stop();
  _thread.join();
}
