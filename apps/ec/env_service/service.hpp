/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_ENV_SERVICE_SERVICE_HPP_
#define APPS_EC_ENV_SERVICE_SERVICE_HPP_

#include <atomic>
#include "srp/env/EnvAppSkeleton.h"
#include "mw/i2c_service/controller/adcsensor/controller.hpp"

using EnvAppSkeleton = srp::env::EnvAppSkeleton;

namespace srp {
namespace apps {

class MyEnvAppSkeleton: public EnvAppSkeleton {
 public:
  explicit MyEnvAppSkeleton(const ara::core::InstanceSpecifier& instance): EnvAppSkeleton{instance} {
  }
  ~MyEnvAppSkeleton() {
  }
 protected:
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_EC_ENV_SERVICE_SERVICE_HPP_
