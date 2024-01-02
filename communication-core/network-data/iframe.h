/**
 * @file iframe.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief This file define interface for all frame
 * @version 1.0
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef COMMUNICATION_CORE_NETWORK_DATA_IFRAME_H_
#define COMMUNICATION_CORE_NETWORK_DATA_IFRAME_H_
#include <cstdint>
#include <memory>
#include <vector>

#include "core/common/error_code.h"

namespace simba {
namespace com {
namespace core {
namespace interface {
class IFrame {
 protected:
  uint8_t size_;
  inline void SetSize(const uint8_t& size) { this->size_ = size; }

 public:
  IFrame() : size_{0} {}
  virtual uint8_t GetSize() const { return this->size_; }
  explicit IFrame(const uint8_t& size) : size_(size) {}
  virtual std::vector<std::uint8_t> GetBuffor() const = 0;
  virtual simba::core::ErrorCode SetBuffor(std::vector<std::uint8_t> data) = 0;
  virtual ~IFrame() = default;
};
}  // namespace interface
}  // namespace core
}  // namespace com
}  // namespace simba

#endif  // COMMUNICATION_CORE_NETWORK_DATA_IFRAME_H_
