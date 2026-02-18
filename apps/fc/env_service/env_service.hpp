/**
 * @file env_service.hpp
 * @author Grzegorz Horbaczewski (gregority9@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_FC_ENV_SERVICE_ENV_SERVICE_HPP_
#define APPS_FC_ENV_SERVICE_ENV_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>
#include <utility>

#include "mw/temp/controller/temp_controller.h"
#include "ara/exec/adaptive_application.h"
#include "mw/i2c_service/controller/bme280/controller.hpp"
#include "srp/env/EnvAppFcSkeleton.h"


namespace srp {
namespace envServiceFc {

class EnvServiceFc final : public ara::exec::AdaptiveApplication {
 private:
  std::unique_ptr<mw::temp::TempController> temp_{};
  std::shared_ptr<i2c::BME280> bme{};

  std::unordered_map<std::uint8_t, std::pair<std::string, std::string>> sensorIdsToPaths{};
  // [sensor_id] = {name, physical_id}
  
  env::EnvAppFcSkeleton service_ipc;
  env::EnvAppFcSkeleton service_udp;
  int LoadTempConfig(
    const std::map<ara::core::StringView, ara::core::StringView>& parms);
  void TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data);

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  core::ErrorCode Init(std::unique_ptr<mw::temp::TempController> temp);
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
                      


 public:
  ~EnvServiceFc() = default;
  EnvServiceFc();
};

}  // namespace envServiceFc
}  // namespace srp

#endif  // APPS_FC_ENV_SERVICE_ENV_SERVICE_HPP_
