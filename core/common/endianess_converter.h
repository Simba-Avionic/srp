/**
 * @file endianess_converter.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define converter which convert from little endian to big
 * endian and from big endian to little endian
 * @version 1.0
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef CORE_COMMON_ENDIANESS_CONVERTER_H_
#define CORE_COMMON_ENDIANESS_CONVERTER_H_
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

namespace srp {
namespace core {
namespace converter {
class EndianessConverter {
 public:
  template <typename T>
  static T Convert(T data) {
    std::array<std::uint8_t, sizeof(T)> buffor;
    std::memcpy(&buffor, &data, sizeof(T));
    std::reverse(buffor.begin(), buffor.end());
    std::memcpy(&data, &buffor, sizeof(T));
    return data;
  }
};
}  // namespace converter
}  // namespace core
}  // namespace srp
#endif  // CORE_COMMON_ENDIANESS_CONVERTER_H_
