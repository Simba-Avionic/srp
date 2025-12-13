/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_EC_ENGINE_SERVICE_SERVICE_HPP_
#define APPS_EC_ENGINE_SERVICE_SERVICE_HPP_

#include <memory>

#include "srp/apps/EngineServiceSkeleton.h"
#include "srp/apps/PrimerService/PrimerServiceHandler.h"
#include "srp/apps/ServoService/ServoServiceHandler.h"
#include "apps/fc/main_service/rocket_state.h"
#include "apps/ec/engine_service/state_controller.hpp"
#include "mw/gpio_server/controller/gpio_controller.hpp"

namespace srp {
namespace apps {
namespace service {


class MyEngineServiceSkeleton: public EngineServiceSkeleton {
 private:
  std::shared_ptr<PrimerServiceHandler> primer_handler_;
  std::shared_ptr<ServoServiceHandler> servo_handler_;
  std::shared_ptr<engineApp::EngineStateController> state_ctr_;
  std::unique_ptr<gpio::IGPIOController> gpio_;
 public:
  explicit MyEngineServiceSkeleton(ara::core::InstanceSpecifier instance);
  void Init(std::shared_ptr<PrimerServiceHandler> primer_handler,
            std::shared_ptr<ServoServiceHandler> servo_handler);
 protected:
  ara::core::Result<bool> Start() override;
  ara::core::Result<bool> SetMode(const std::uint8_t& in_parm) override;
};

}  // namespace service
}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENGINE_SERVICE_SERVICE_HPP_
