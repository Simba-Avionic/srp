/**
 * @file logger_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
#define APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>
#include <mutex>  // NOLINT
#include <atomic>

#include "ara/exec/adaptive_application.h"
#include "ara/log/logging_menager.h"
#include "srp/env/EnvApp/EnvAppHandler.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "core/csvdriver/csvdriver.h"
#include "apps/ec/logger_service/data_type.hpp"
#include "apps/ec/logger_service/service/service.hpp"
#include "apps/ec/logger_service/service/logger_did.hpp"
#include "srp/apps/SysStatService/SysStatServiceHandler.h"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "srp/apps/EngineService/EngineServiceHandler.h"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace srp {
namespace logger {

class LoggerService final : public ara::exec::AdaptiveApplication {
 private:
  const ara::log::Logger& someip_logger;
  env::EnvAppProxy env_service_proxy;
  gpio::GPIOController gpio_;
  apps::SysStatServiceProxy stat_service_proxy;
  apps::PrimerServiceProxy primer_service_proxy;
  apps::ServoServiceProxy servo_service_proxy;
  apps::EngineServiceProxy engine_service_proxy;
  std::shared_ptr<env::EnvAppHandler> env_service_handler;
  std::shared_ptr<apps::SysStatServiceHandler> stat_service_handler;
  std::shared_ptr<apps::PrimerServiceHandler> primer_service_handler;
  std::shared_ptr<apps::ServoServiceHandler> servo_service_handler;
  std::shared_ptr<apps::EngineServiceHandler> engine_service_handler;
  Data_t data;
  std::unique_ptr<FileLoggerDID> logger_did_;
  const ara::core::InstanceSpecifier did_instance;
  std::shared_ptr<core::timestamp::TimestampController> timestamp_;

  std::atomic<uint8_t> save_state{0};

  std::shared_ptr<std::jthread> save_thread_;
  void SaveLoop(const std::stop_token& token, std::shared_ptr<core::timestamp::TimestampController> timestamp);
  void start_func_handler(const std::uint8_t status);

  std::unique_ptr<apps::MyFileLoggerAppSkeleton> service_ipc;
  std::unique_ptr<apps::MyFileLoggerAppSkeleton> service_udp;

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
  void SomeIpInit();

 public:
  ~LoggerService();
  LoggerService();
};

}  // namespace logger
}  // namespace srp


#endif  // APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_SERVICE_HPP_
