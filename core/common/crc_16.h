/**
 * @file crc_16.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief CRC calculation for (CITT-FALSE CRC16)
 * @version 0.1
 * @date 2024-01-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_COMMON_CRC_16_H_
#define CORE_COMMON_CRC_16_H_
#include <cstdint>
#include <vector>
namespace srp {
namespace core {
class CRC16 {
 public:
  static const std::uint16_t calculate(const uint8_t& data);
  static const std::uint16_t calculate(const uint16_t& data);
  static const std::uint16_t calculate(const uint32_t& data);
  static const std::uint16_t calculate(const uint64_t& data);
  static const std::uint16_t calculate(const int8_t& data);
  static const std::uint16_t calculate(const int16_t& data);
  static const std::uint16_t calculate(const int32_t& data);
  static const std::uint16_t calculate(const int64_t& data);
  static const std::uint16_t calculate(const std::vector<uint8_t>& data);
};

}  // namespace core
}  // namespace srp
#endif  // CORE_COMMON_CRC_16_H_
