/**
 * @file engine_app.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_ENGINE_SERVICE_ENGINE_APP_HPP_
#define APPS_ENGINE_SERVICE_ENGINE_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "ara/exec/adaptive_application.h"
#include "simba/apps/PrimerService/PrimerServiceHandler.h"
#include "simba/apps/ServoService/ServoServiceHandler.h"
#include "apps/engine_service/service.hpp"


namespace simba {
namespace apps {

class EngineApp final : public ara::exec::AdaptiveApplication {
 private:
  std::shared_ptr<PrimerServiceHandler> primer_handler_;
  std::shared_ptr<ServoServiceHandler> servo_handler_;
  PrimerServiceProxy primer_proxy;
  ServoServiceProxy servo_proxy;

  service::MyEngineServiceSkeleton service_ipc;
  service::MyEngineServiceSkeleton service_udp;

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

 public:
    EngineApp();
};

}  // namespace apps
}  // namespace simba


#endif  // APPS_ENGINE_SERVICE_ENGINE_APP_HPP_
