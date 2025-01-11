/**
 * @file idlt_frame.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_LOGGER_CODE_DATA_IDLT_FRAME_H_
#define PLATFORM_COMMON_LOGGER_CODE_DATA_IDLT_FRAME_H_
#include <optional>
#include <string>
#include <vector>

namespace srp {
namespace dlt {
namespace data {
class IDLTFrame {
 public:
  virtual std::optional<std::vector<uint8_t>> ParseFrame() noexcept = 0;
  virtual ~IDLTFrame() {}
};
}  // namespace data
}  // namespace dlt
}  // namespace srp
#endif  // PLATFORM_COMMON_LOGGER_CODE_DATA_IDLT_FRAME_H_
