#include <iostream>
#include <boost/asio.hpp>
#include "../cmake-build-debug/generated/my_messages.pb.h"
//#include "my_messages.pb.h"

using boU = boost::asio::ip::udp;
namespace boA = boost::asio;

const int port = 8080;
int main() {
  //  std::cout << "Protobuf version: " << GOOGLE_PROTOBUF_VERSION << std::endl;

  const size_t MAX_UDP_PACKET_SIZE = 508;

    boA::io_context io_context;
    boU::socket mySocket(io_context, boU::endpoint(boU::v4(), port));

    while (true) {
        // Receive message from client
        boU::endpoint remote_endpoint;
        char recv_buffer[MAX_UDP_PACKET_SIZE];
        size_t len = mySocket.receive_from(boA::buffer(recv_buffer, MAX_UDP_PACKET_SIZE), remote_endpoint);

        // Deserialize message using protobuf
        VideoFeed video_feed;
        video_feed.ParseFromArray(recv_buffer, len);

        // Print or process received message
        std::cout << "Received video feed message: " << video_feed.messagefeed() << std::endl;

        // Process video frame and determine instruction
        Instruction instruction;
        instruction.set_messageinstruction("This is a response from the server.");

        // Serialize instruction message
        std::string serialized_instruction;
        instruction.SerializeToString(&serialized_instruction);

        // Send instruction back to client
        mySocket.send_to(boA::buffer(serialized_instruction), remote_endpoint);
    }

    return 0;
}