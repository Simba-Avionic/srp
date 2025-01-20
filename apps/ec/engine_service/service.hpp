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

namespace srp {
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
  MODE_t mode_ = MODE_t::AUTO;
 public:
  explicit MyEngineServiceSkeleton(ara::core::InstanceSpecifier instance);
  void Init(std::shared_ptr<PrimerServiceHandler> primer_handler,
            std::shared_ptr<ServoServiceHandler> servo_handler);
  MODE_t GetMode() const;
 protected:
  ara::core::Result<bool> Start() override;
  ara::core::Result<bool> SetMode(const std::uint8_t& in_parm) override;
};

}  // namespace service
}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENGINE_SERVICE_SERVICE_HPP_
