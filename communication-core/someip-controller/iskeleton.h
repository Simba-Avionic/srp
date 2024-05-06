/**
 * @file iskeleton.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISKELETON_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISKELETON_H_
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <utility>

#include "communication-core/someip-controller/callbacks.h"
#include "communication-core/someip-database/code/endpoint.h"
namespace simba {
namespace com {
namespace someip {
class ISkeleton {
 public:
  virtual void SetCallback(SendCallback callback) {}
  virtual std::optional<objects::Endpoint> GetEndPoint() const noexcept = 0;
  virtual std::pair<data::MessageCode, std::optional<std::vector<uint8_t>>> Call(
      std::vector<uint8_t>, const objects::Endpoint) noexcept = 0;
  virtual std::string GetName() const = 0;
  virtual ~ISkeleton() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_ISKELETON_H_
