/**
 * @file env_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Secondary EC environment service application
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef APPS_SEC_EC_ENV_SERVICE_ENV_SERVICE_HPP_
#define APPS_SEC_EC_ENV_SERVICE_ENV_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <mutex>  // NOLINT

#include "mw/temp/controller/temp_controller.h"
#include "ara/exec/adaptive_application.h"
#include "apps/sec_ec/env_service/service.hpp"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"
#include "mw/i2c_service/controller/24lc32at/eeprom_config/cfg_manager.hpp"

namespace srp {
namespace sec_ec {

class SecEnvService final : public ara::exec::AdaptiveApplication {
 private:
  eeprom::ConfigManager config;
  std::unique_ptr<mw::temp::TempController> temp_{};
  std::shared_ptr<i2c::ADCSensorController> press_{};
  std::mutex press_mtx_;
  std::unordered_map<std::uint8_t, std::pair<std::string, std::string>> sensorIdsToPaths{};

  std::mutex service_mtx_;
  apps::MySecEnvAppSkeleton service_ipc;
  apps::MySecEnvAppSkeleton service_udp;

  core::ErrorCode Init(std::unique_ptr<mw::temp::TempController> temp);
  core::ErrorCode LoadTempConfig(
      const std::map<ara::core::StringView, ara::core::StringView>& parms);
  void GenericPressureLoop(
      const std::stop_token& token,
      uint8_t sensorId,
      std::chrono::milliseconds delay,
      const std::string& label,
      auto& eventIpc,  // NOLINT
      auto& eventUdp);  // NOLINT
  void TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data);

 protected:
  int Run(const std::stop_token& token) override;
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

 public:
  ~SecEnvService() = default;
  SecEnvService();
};

}  // namespace sec_ec
}  // namespace srp

#endif  // APPS_SEC_EC_ENV_SERVICE_ENV_SERVICE_HPP_
