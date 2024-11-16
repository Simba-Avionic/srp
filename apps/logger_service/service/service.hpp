/**
 * @file service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_LOGGER_SERVICE_SERVICE_SERVICE_HPP_
#define APPS_LOGGER_SERVICE_SERVICE_SERVICE_HPP_

#include "simba/apps/FileLoggerAppSkeleton.h"

using SaveThreadHandler =
      std::function<void(const std::uint8_t status)>;

namespace simba {
namespace apps {

class MyFileLoggerAppSkeleton: public FileLoggerAppSkeleton {
 private:
    SaveThreadHandler handler;
    std::stop_token token;

 public:
  MyFileLoggerAppSkeleton(const ara::core::InstanceSpecifier& instance, SaveThreadHandler handler):
        FileLoggerAppSkeleton{instance}, handler(handler) {
  }
  ~MyFileLoggerAppSkeleton() {
  }

 protected:
  ara::core::Result<bool> Start() override {
    this->handler(1);
    return ara::core::Result<bool>(true);
  }
  ara::core::Result<bool> Stop() override {
    this->handler(0);
    return ara::core::Result<bool>(true);
  }
};
}  // namespace apps
}  // namespace simba

#endif  // APPS_LOGGER_SERVICE_SERVICE_SERVICE_HPP_
