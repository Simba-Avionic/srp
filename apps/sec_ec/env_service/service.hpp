/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief Secondary EC environment service skeleton
 * @version 0.1
 * @date 2026-07-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef APPS_SEC_EC_ENV_SERVICE_SERVICE_HPP_
#define APPS_SEC_EC_ENV_SERVICE_SERVICE_HPP_

#include "srp/env/SecEnvAppSkeleton.h"

using SecEnvAppSkeleton = srp::env::SecEnvAppSkeleton;

namespace srp {
namespace apps {

class MySecEnvAppSkeleton : public SecEnvAppSkeleton {
 public:
  explicit MySecEnvAppSkeleton(const ara::core::InstanceSpecifier& instance)
      : SecEnvAppSkeleton{instance} {}
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_SEC_EC_ENV_SERVICE_SERVICE_HPP_
