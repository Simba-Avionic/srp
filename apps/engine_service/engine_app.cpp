/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/engine_service/engine_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
namespace apps {

EngineApp::EngineApp(): primer_proxy(ara::core::InstanceSpecifier{"simba/apps/PrimerService/PrimService_ipc"}),
  servo_proxy((ara::core::InstanceSpecifier{"simba/apps/servoService/ServoService_ipc"})),
  primer_handler_{nullptr}, servo_handler_{nullptr}, mode(std::make_shared<service::MODE_t>()),
  service_ipc(ara::core::InstanceSpecifier{"simba/apps/EngineService/EngineService_ipc"},
  primer_handler_, servo_handler_, mode), service_udp(ara::core::InstanceSpecifier{
  "simba/apps/EngineService/EngineService_udp"}, primer_handler_, servo_handler_, mode) {
    *mode = service::MODE_t::AUTO;
}

int EngineApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "Run complete, closing";
  core::condition::wait(token);
  servo_proxy.StopFindService();
  primer_proxy.StopFindService();
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
  servo_proxy.StartFindService([this](auto handler) {
    this->servo_handler_ = handler;
  });
  primer_proxy.StartFindService([this](auto handler) {
    this->primer_handler_ = handler;
  });
  ara::log::LogInfo() << "Initialize Complete";
}

}  // namespace apps
}  // namespace simba
