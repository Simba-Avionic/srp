/**
 * @file method_skeleton.h
 * @author Bartosz Snieg (snieg45@gmial.com)
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_SKELETON_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_SKELETON_H_

#include <condition_variable>  // NOLINT
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "communication-core/someip-controller/callbacks.h"
#include "communication-core/someip-controller/iskeleton.h"
#include "communication-core/someip-database/code/com_config.h"
#include "communication-core/someip-database/code/endpoint.h"
#include "communication-core/someip-database/code/interface.h"
#include "communication-core/someip/message_code.h"
#include "communication-core/someip/message_type.h"
#include "core/logger/Logger.h"

namespace simba {
namespace com {
namespace someip {
class MethodSkeleton : public ISkeleton {
 private:
  const std::optional<objects::Endpoint> object;
  MethodCallback callback_;
  const std::string instance_;

 public:
  std::optional<objects::Endpoint> GetEndPoint() const noexcept override {
    return object;
  }
  std::optional<std::vector<uint8_t>> Call(
      std::vector<uint8_t> payload) noexcept override {
    return callback_(std::move(payload));
  }
  std::string GetName() const override { return instance_; }

  MethodSkeleton(const std::string instance, MethodCallback callback)
      : object{std::move(com::config::ComConfig::FindObject(instance))},
        callback_{callback},
        instance_{std::move(instance)} {
    if (!object.has_value()) {
      AppLogger::Error("[ SOMEIP SKELETON  ]: Can't find object for: " +
                       instance);
    } else {
      AppLogger::Info("[ SOMEIP SKELETON ]: " + instance_ + " mapped to " +
                      std::to_string(object.value().GetServiceId()) + "/" +
                      std::to_string(object.value().GetEndpointId()));
    }
  }
  ~MethodSkeleton() = default;
};
}  // namespace someip
}  // namespace com
}  // namespace simba
#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_METHOD_SKELETON_H_
