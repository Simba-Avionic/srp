/**
 * @file logger_did.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
#define APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
#include <string>
#include <vector>
#include <memory>
#include <strstream>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "ara/com/com_error_domain.h"
#include "ara/diag/generic_data_identifier.h"
namespace srp {
namespace logger {

using SaveThreadHandler =
      std::function<void(const std::uint8_t status)>;

class FileLoggerDID : public ara::diag::GenericDiD {
 private:
  SaveThreadHandler handler_;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    // return diag::DiagResponse(diag::DiagResponseCodes::kSubFunctionNotSupported);
  }
  ara::core::Result<void> Write(
      const std::vector<uint8_t> &payload) noexcept override {
   ara::log::LogError() << "Receive write req";
  if (payload.size() != 1) {
    return {};
  }
  handler_(payload[0]);
  return {};
  }

 public:
  FileLoggerDID(const ara::core::InstanceSpecifier& instance, SaveThreadHandler handler)
                        : ara::diag::GenericDiD{instance}, handler_(handler) {
      }
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_EC_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
