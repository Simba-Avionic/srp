/**
 * @file dtc_msg_factory.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "temp_reading_msg_factory.h"

#include <algorithm>
#include <utility>
#include <array>
#include <cstring>
namespace srp {
namespace mw {
namespace temp {

std::vector<uint8_t> TempReadingMsgFactory::GetBuffer(
  std::vector<std::pair<uint8_t, double>> payload) {
  std::vector<uint8_t> res;

  for (const auto& pair : payload) {
      uint8_t first = pair.first;
      double second = pair.second;
      std::array<uint8_t, sizeof(uint8_t)> firstBytes;
      std::array<uint8_t, sizeof(double)> secondBytes;

      memcpy(firstBytes.data(), &first, sizeof(uint8_t));
      memcpy(secondBytes.data(), &second, sizeof(double));

      res.insert(res.end(), firstBytes.begin(), firstBytes.end());
      res.insert(res.end(), secondBytes.begin(), secondBytes.end());
  }

  return res;
}

std::vector<std::pair<uint8_t, double>>
  TempReadingMsgFactory::GetPayload(std::vector<uint8_t> raw_data) {
    std::vector<std::pair<uint8_t, double>> payload{};
    if (raw_data.size() > 4) {
      for (size_t i = 0; i < raw_data.size(); i += (sizeof(uint8_t) + sizeof(double))) {
          uint8_t first;
          std::memcpy(&first, &raw_data[i], sizeof(uint8_t));

          double second;
          std::memcpy(&second, &raw_data[i+sizeof(uint8_t)], sizeof(double));

          payload.push_back(std::make_pair(first, second));
      }
    return payload;
    }
    return {};
}

}  // namespace temp
}  // namespace mw
}  // namespace srp
