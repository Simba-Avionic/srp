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
#ifndef MW_LOGGER_CODE_DATA_IDLT_FRAME_H_
#define MW_LOGGER_CODE_DATA_IDLT_FRAME_H_
#include <string>
#include <vector>

#include "core/results/result.h"
namespace simba {
namespace dlt {
namespace data {
class IDLTFrame {
 public:
  virtual core::Result<std::vector<uint8_t>> ParseFrame() noexcept = 0;
  virtual ~IDLTFrame() {}
};
}  // namespace data
}  // namespace dlt
}  // namespace simba
#endif  // MW_LOGGER_CODE_DATA_IDLT_FRAME_H_
