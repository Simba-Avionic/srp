/**
 * @file logger_did.hpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
#define APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"

namespace srp {
namespace logger {

using DidSaveThreadHandler = std::function<void(std::uint8_t status)>;

class FileLoggerDID : public ara::diag::GenericDiD {
 private:
  DidSaveThreadHandler handler_;

  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
  }

  ara::core::Result<void> Write(const std::vector<std::uint8_t>& payload) noexcept override {
    if (payload.size() != 1U) {
      return ara::diag::MakeErrorCode(
          ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
    }

    handler_(payload[0]);
    return {};
  }

 public:
  FileLoggerDID(const ara::core::InstanceSpecifier& instance, DidSaveThreadHandler handler)
      : ara::diag::GenericDiD{instance}, handler_(std::move(handler)) {}
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
