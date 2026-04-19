/**
 * @file logger_builder.hpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_
#define APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "apps/fc/logger_service/service/logger_did.hpp"
#include "apps/fc/logger_service/service/service.hpp"

namespace srp {
namespace logger {

class Builder {
 private:
  std::unique_ptr<FileLoggerDID> logger_did_;
  std::unique_ptr<apps::MyFcFileLoggerAppSkeleton> service_ipc_;
  std::unique_ptr<apps::MyFcFileLoggerAppSkeleton> service_udp_;
  std::function<void(std::uint8_t)> callback_;

 public:
  explicit Builder(std::function<void(std::uint8_t)> callback_handler)
      : callback_(std::move(callback_handler)) {}

  Builder& SetLoggerDID(const ara::core::InstanceSpecifier& specifier) {
    logger_did_ = std::make_unique<FileLoggerDID>(specifier, callback_);
    return *this;
  }

  Builder& SetLoggerIPC(const std::string& instance) {
    service_ipc_ = std::make_unique<apps::MyFcFileLoggerAppSkeleton>(
        ara::core::InstanceSpecifier{instance},
        callback_);
    return *this;
  }

  Builder& SetLoggerUDP(const std::string& instance) {
    service_udp_ = std::make_unique<apps::MyFcFileLoggerAppSkeleton>(
        ara::core::InstanceSpecifier{instance},
        callback_);
    return *this;
  }

  struct Result {
    std::unique_ptr<FileLoggerDID> logger_did;
    std::unique_ptr<apps::MyFcFileLoggerAppSkeleton> service_ipc;
    std::unique_ptr<apps::MyFcFileLoggerAppSkeleton> service_udp;
  };

  Result Build() {
    return {std::move(logger_did_), std::move(service_ipc_), std::move(service_udp_)};
  }
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_BUILDER_HPP_
