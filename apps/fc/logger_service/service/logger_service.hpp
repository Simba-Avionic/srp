/**
 * @file logger_service.hpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
#define APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>

#include "apps/fc/logger_service/data_type.hpp"
#include "apps/fc/logger_service/service/service.hpp"
#include "ara/exec/adaptive_application.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/apps/FcSysStatService/FcSysStatServiceHandler.h"
#include "srp/env/EnvAppFc/EnvAppFcHandler.h"

namespace srp {
namespace logger {

class LoggerService final : public ara::exec::AdaptiveApplication {
 private:
  env::EnvAppFcProxy env_service_proxy_;
  apps::FcSysStatServiceProxy stat_service_proxy_;
  std::shared_ptr<env::EnvAppFcHandler> env_service_handler_;
  std::shared_ptr<apps::FcSysStatServiceHandler> stat_service_handler_;
  Data_t data_;
  std::shared_ptr<core::timestamp::TimestampController> timestamp_;

  std::atomic<uint8_t> save_state_{0};
  std::shared_ptr<std::jthread> save_thread_;

  std::unique_ptr<apps::MyFcFileLoggerAppSkeleton> service_ipc_;

  void SaveLoop(const std::stop_token& token);
  void StartFuncHandler(std::uint8_t status);
  void SomeIpInit();

 protected:
  int Run(const std::stop_token& token) override;
  int Initialize(
      const std::map<ara::core::StringView, ara::core::StringView> parms) override;

 public:
  ~LoggerService() override;
  LoggerService();
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
