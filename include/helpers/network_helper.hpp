#ifndef REALTIMESYSROVER_NETWORK_HELPER_HPP
#define REALTIMESYSROVER_NETWORK_HELPER_HPP

#include <array>

#include "my_messages.pb.h"
#include "nlohmann/json.hpp"
#include "opencv2/opencv.hpp"

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
  std::array<unsigned char, 4> bytes {};

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
 * @return int The integer
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
 * @brief Decodes a base64 string
 *
 * @param in The base64 string to decode
 * @return std::string The decoded string
 */
inline std::string base64_decode(const std::string &in) {
  std::string out;
  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

  int val = 0, valb = -8;
  for (uchar c : in) {
    if (T[c] == -1) break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0) {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

/**
 * @brief Encodes an image to a protobuf message
 *
 * @param frame The image to encode
 * @return cv::Mat The encoded protobuf message
 */
inline cv::Mat decodeImageFromProto(const std::string &frame) {
  VideoFeed video_feed;
  video_feed.ParseFromString(frame);

  std::vector<uchar> encoded_frame(video_feed.messagefeed().begin(), video_feed.messagefeed().end());
  cv::Mat decoded_frame = cv::imdecode(encoded_frame, cv::IMREAD_COLOR);
  return decoded_frame;
}

/**
 * @brief Encodes an image to a JSON string
 *
 * @param jsonString The JSON string to decode
 * @return cv::Mat The decoded image
 */
inline cv::Mat decodeImageFromJson(const std::string &jsonString) {
  // Parse the JSON
  auto json = nlohmann::json::parse(jsonString);
  std::string encoded_image = json["image"];

  // Base64 Decode
  std::string decodedImageData = base64_decode(encoded_image);

  // Convert to vector of bytes
  std::vector<uchar> data(decodedImageData.begin(), decodedImageData.end());

  // Decode image
  cv::Mat image = cv::imdecode(data, cv::IMREAD_COLOR);

  return image;
}


#endif//REALTIMESYSROVER_NETWORK_HELPER_HPP
