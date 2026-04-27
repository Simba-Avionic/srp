/**
 * @file service.hpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_FC_LOGGER_SERVICE_SERVICE_SERVICE_HPP_
#define APPS_FC_LOGGER_SERVICE_SERVICE_SERVICE_HPP_

#include <functional>

#include "srp/apps/FcFileLoggerAppSkeleton.h"

using SaveThreadHandler = std::function<void(const std::uint8_t status)>;

namespace srp {
namespace apps {

class MyFcFileLoggerAppSkeleton : public FcFileLoggerAppSkeleton {
 private:
  SaveThreadHandler handler_;

 public:
  MyFcFileLoggerAppSkeleton(
      const ara::core::InstanceSpecifier& instance,
      SaveThreadHandler handler)
      : FcFileLoggerAppSkeleton{instance}, handler_(handler) {}

  ~MyFcFileLoggerAppSkeleton() override = default;

 protected:
  ara::core::Result<bool> Start() override {
    handler_(1);
    return ara::core::Result<bool>(true);
  }

  ara::core::Result<bool> Stop() override {
    handler_(0);
    return ara::core::Result<bool>(true);
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_SERVICE_SERVICE_HPP_
