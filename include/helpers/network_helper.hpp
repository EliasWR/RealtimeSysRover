#ifndef REALTIMESYSROVER_NETWORK_HELPER_HPP
#define REALTIMESYSROVER_NETWORK_HELPER_HPP

#include <array>

#include "my_messages.pb.h"

enum class byte_order {
  LITTLE,
  BIG
};

/**
 * @brief Converts an integer to a byte array
 *
 * @param n The integer to convert
 * @param order The byte order to use
 * @return The byte array
 */
inline std::array<unsigned char, 4> int_to_bytes(int n, byte_order order = byte_order::LITTLE) {
  std::array<unsigned char, 4> bytes{};

  if (order == byte_order::LITTLE) {
    bytes[0] = n & 0xFF;
    bytes[1] = (n >> 8) & 0xFF;
    bytes[2] = (n >> 16) & 0xFF;
    bytes[3] = (n >> 24) & 0xFF;
  }
  else {
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
  }

  return bytes;
}

/**
 * @brief Converts a byte array to an integer
 *
 * @param buffer The byte array to convert
 * @param order The byte order to use
 * @return The integer
 */
inline int bytes_to_int(std::array<unsigned char, 4> buffer, byte_order order = byte_order::LITTLE) {
  if (order == byte_order::LITTLE) {
    return int(buffer[0] |
               buffer[1] << 8 |
               buffer[2] << 16 |
               buffer[3] << 24);
  }
  else {
    return int(buffer[0] << 24 |
               buffer[1] << 16 |
               buffer[2] << 8 |
               buffer[3]);
  }
}

/**
 * @brief Encodes an image to a protobuf message
 *
 * @param frame The image to encode
 * @return The encoded protobuf message
 */
cv::Mat decodeImageFromProto(const std::string &frame) {
  VideoFeed video_feed;
  video_feed.ParseFromString(frame);

  std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
  cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
  return decoded_frame;
}
#endif//REALTIMESYSROVER_NETWORK_HELPER_HPP
