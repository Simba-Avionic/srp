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
#include <cstring>
namespace simba {
namespace mw {
namespace temp {

std::vector<uint8_t> TempReadingMsgFactory::GetBuffer(
  std::vector<std::pair<uint16_t, double>> payload) {
  std::vector<std::uint8_t> res;

  for (const auto& pair : payload) {
      uint16_t first = pair.first;
      double second = pair.second;

      uint8_t firstBytes[sizeof(uint16_t)];
      uint8_t secondBytes[sizeof(double)];

      memcpy(firstBytes, &first, sizeof(uint16_t));
      memcpy(secondBytes, &second, sizeof(double));

      res.insert(res.end(), firstBytes, firstBytes + sizeof(uint16_t));
      res.insert(res.end(), secondBytes, secondBytes + sizeof(double));
  }

  return res;
}

std::vector<std::pair<uint16_t, double>>
  TempReadingMsgFactory::GetPayload(std::vector<uint8_t> raw_data) {
    std::vector<std::pair<uint16_t, double>> payload{};
    if (raw_data.size() > 4) {
      for (size_t i = 0; i < raw_data.size(); i += (sizeof(uint16_t) + sizeof(double))) {
          uint16_t first;
          std::memcpy(&first, &raw_data[i], sizeof(uint16_t));

          double second;
          std::memcpy(&second, &raw_data[i+sizeof(uint16_t)], sizeof(double));

          payload.push_back(std::make_pair(first, second));
      }
    return payload;
    }
    return {};
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
