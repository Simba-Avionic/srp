/**
 * @file i_controller.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "ara/com/someip/someip_frame.h"
#include "ara/core/result.h"
namespace srp {
namespace someip_demon {
namespace common {
class IController {
 public:
  using TransmitCallback =
      std::function<void(const std::uint32_t& ip, const std::uint16_t& port,
                         const ara::com::someip::SomeipFrame& frame)>;
  virtual ~IController() = default;
  virtual void SetTransmitCallback(TransmitCallback callback) noexcept = 0;
  virtual ara::core::Result<void> Start() noexcept = 0;
  virtual void Stop() noexcept = 0;

  virtual void ProcessFrame(
      const std::string& ip, const std::uint16_t& port,
      const ara::com::someip::SomeipFrame& frame) noexcept = 0;
};
}  // namespace common
}  // namespace someip_demon
}  // namespace srp

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_COMMON_I_CONTROLLER_H_
