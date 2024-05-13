/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include <memory>
#include <vector>
#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/logger/Logger.h"
#include "diag/dtc/controller/dtc.h"
#include "mw/i2c_service/data/i2c_factory.h"
namespace simba {
namespace router {

namespace {
  const constexpr uint8_t BME280_ADDR = 0x50;
  const constexpr uint8_t BME280_TEMP_REG = 0xFA;
}

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
  auto res = this->i2c_.Read(BME280_ADDR, 0x00, 3);
  AppLogger::Warning("Receive response");
  if (!res.has_value()) {
    AppLogger::Warning("NO VALUE IN RES");
  } else {
  AppLogger::Warning("RES:"+std::to_string(res.value()[0])+":"
  +std::to_string(res.value()[1])+":"+std::to_string(res.value()[2]));
  auto hdr = i2c::I2CFactory::GetHeader(res.value());
  auto payload = i2c::I2CFactory::GetPayload(res.value());

  std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  }
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->i2c_.Init(12);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
