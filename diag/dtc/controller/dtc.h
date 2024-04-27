/**
 * @file dtc.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DIAG_DTC_CONTROLLER_DTC_H_
#define DIAG_DTC_CONTROLLER_DTC_H_

#include "diag/dtc/controller/i_dtc.h"

namespace simba {
namespace diag {
namespace dtc {
class DTCObject : public IDTC {
 protected:
  const uint16_t id;
  DTCSendCallback callback;

 public:
  void Pass() const noexcept { callback(id, 2U); }
  void Failed() const noexcept { callback(id, 1U); }
  explicit DTCObject(const uint16_t id_) : id{id_} {}
  void SetCallback(DTCSendCallback callback_) noexcept override {
    callback = callback_;
    callback(id, 0U);
  }
  uint16_t GetId() const override { return id; }
};

}  // namespace dtc
}  // namespace diag
}  // namespace simba

#endif  // DIAG_DTC_CONTROLLER_DTC_H_
