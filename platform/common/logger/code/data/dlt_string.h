/**
 * @file dlt_string.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_STRING_H_
#define PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_STRING_H_
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "platform/common/logger/code/data/idlt_arg.h"
namespace srp {
namespace dlt {
namespace data {
class DltString : IDLTArg {
 private:
  std::string log{""};

 public:
  DltString() = default;
  explicit DltString(const srp::dlt::data::DltString& other) {
    log = other.log;
  }
  DltString operator=(const srp::dlt::data::DltString& other) {
    return DltString{other.log};
  }
  explicit DltString(const std::string& log_) { this->log = log_; }
  uint16_t Length() const noexcept override { return 0x07 + log.size(); }
  std::vector<uint8_t> ParseArg() const noexcept override {
    const auto p_length = static_cast<uint16_t>(log.size()) + 1;
    std::vector<uint8_t> res{0x00,
                             0x00,
                             0x82,
                             0x00,
                             static_cast<uint8_t>(p_length >> 8U),
                             static_cast<uint8_t>(p_length & 0xFF)};
    std::copy(log.begin(), log.end(), std::back_inserter(res));
    res.push_back(0x00);
    return res;
  }
};
}  // namespace data
}  // namespace dlt
}  // namespace srp

#endif  // PLATFORM_COMMON_LOGGER_CODE_DATA_DLT_STRING_H_
