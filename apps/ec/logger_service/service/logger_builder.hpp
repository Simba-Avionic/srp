/**
 * @file logger_builder.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_
#define APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_

#include <memory>
#include <utility>
#include <string>
#include "apps/ec/logger_service/service/service.hpp"
#include "apps/ec/logger_service/service/logger_did.hpp"
#include "ara/diag/generic_data_identifier.h"

namespace srp {
namespace logger {

class Builder {
 private:
  std::unique_ptr<FileLoggerDID> logger_did_;
  std::unique_ptr<apps::MyFileLoggerAppSkeleton> service_ipc;
  std::unique_ptr<apps::MyFileLoggerAppSkeleton> service_udp;
  std::function<void(uint8_t)> callback;

 public:
  explicit Builder(std::function<void(uint8_t)> callbackHandler):
    callback(callbackHandler) {}

  Builder& setLoggerDID(const ara::core::InstanceSpecifier& specifier) {
    logger_did_ = std::make_unique<FileLoggerDID>(specifier, callback);
    return *this;
  }
  Builder& setLoggerIPC(const std::string& instance) {
    service_ipc = std::make_unique<apps::MyFileLoggerAppSkeleton>(ara::core::InstanceSpecifier{instance}, callback);
    return *this;
  }
  Builder& setLoggerUDP(const std::string& instance) {
    service_udp = std::make_unique<apps::MyFileLoggerAppSkeleton>(ara::core::InstanceSpecifier{instance}, callback);
    return *this;
  }
  struct Result {
    std::unique_ptr<FileLoggerDID> loggerDID;
    std::unique_ptr<apps::MyFileLoggerAppSkeleton> serviceIPC;
    std::unique_ptr<apps::MyFileLoggerAppSkeleton> serviceUDP;
  };

    Result build() {
        return {std::move(logger_did_), std::move(service_ipc), std::move(service_udp)};
    }
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_
