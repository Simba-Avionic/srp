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
#ifndef APPS_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
#define APPS_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
#include <string>
#include <vector>
#include <memory>
#include <strstream>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "ara/com/com_error_domain.h"
#include "diag/jobs/skeleton/did_job.h"
namespace simba {
namespace logger {

using SaveThreadHandler =
      std::function<void(const std::uint8_t status)>;

class FileLoggerDID : public diag::DiDJob {
 private:
  SaveThreadHandler handler_;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    return diag::DiagResponse(diag::DiagResponseCodes::kSubFunctionNotSupported);
  }
diag::DiagResponse Write(const std::vector<uint8_t>& payload) {
  ara::log::LogError() << "Receive write req";
  if (payload.size() != 1) {
    return diag::DiagResponse{diag::DiagResponseCodes::kInvalidMessageLengthFormat};
  }
  handler_(payload[0]);
  return diag::DiagResponse{diag::DiagResponseCodes::kOk};
}

 public:
  FileLoggerDID(const ara::core::InstanceSpecifier& instance, SaveThreadHandler handler)
                        : diag::DiDJob(instance), handler_(handler) {
      }
};

}  // namespace logger
}  // namespace simba

#endif  // APPS_LOGGER_SERVICE_SERVICE_LOGGER_DID_HPP_
