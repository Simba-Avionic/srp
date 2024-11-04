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

#ifndef APPS_ENGINE_SERVICE_SERVICE_HPP_
#define APPS_ENGINE_SERVICE_SERVICE_HPP_

#include <memory>

#include "simba/apps/EngineServiceSkeleton.h"
#include "simba/apps/PrimerService/PrimerServiceHandler.h"
#include "simba/apps/ServoService/ServoServiceHandler.h"

namespace simba {
namespace apps {
namespace service {

enum MODE_t {
    AUTO = 0,
    MAN = 1,
    ERROR = 2,
};

class MyEngineServiceSkeleton: public EngineServiceSkeleton {
 private:
  std::shared_ptr<PrimerServiceHandler> primer_handler_;
  std::shared_ptr<ServoServiceHandler> servo_handler_;
  std::shared_ptr<MODE_t> mode_;
 public:
  MyEngineServiceSkeleton(const ara::core::InstanceSpecifier& instance,
   std::shared_ptr<PrimerServiceHandler> primer_handler,
   std::shared_ptr<ServoServiceHandler> servo_handler, std::shared_ptr<MODE_t> mode);
 protected:
  ara::core::Result<bool> Start() override;
  ara::core::Result<bool> SetMode(const std::uint8_t& in_parm) override;
};

}  // namespace service
}  // namespace apps
}  // namespace simba

#endif  // APPS_ENGINE_SERVICE_SERVICE_HPP_
