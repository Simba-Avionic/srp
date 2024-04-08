/**
 * @file i_dtc.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DIAG_DTC_CONTROLLER_I_DTC_H_
#define DIAG_DTC_CONTROLLER_I_DTC_H_

#include <cstdint>
#include <functional>

#include "core/common/error_code.h"

namespace simba {
namespace diag {
namespace dtc {
using DTCSendCallback =
    std::function<void(const uint8_t& dtc_id, const uint8_t& flag)>;

class IDTC {
 public:
  virtual void SetCallback(DTCSendCallback callback) noexcept = 0;
  virtual uint16_t GetId() const = 0;
};

}  // namespace dtc
}  // namespace diag
}  // namespace simba
#endif  // DIAG_DTC_CONTROLLER_I_DTC_H_
