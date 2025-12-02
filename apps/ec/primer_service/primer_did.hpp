/**
 * @file primer_did.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APPS_EC_PRIMER_SERVICE_PRIMER_DID_HPP_
#define APPS_EC_PRIMER_SERVICE_PRIMER_DID_HPP_
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include "ara/log/log.h"
#include "ara/com/com_error_domain.h"
#include "ara/diag/generic_data_identifier.h"
#include "ara/diag/uds_error_domain.h"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
namespace srp {
namespace primer {


class PrimerDID : public ara::diag::GenericDiD {
 private:
  std::shared_ptr<PrimerController> handler_;
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  ara::core::Result<ara::diag::OperationOutput> Read() noexcept override {
    return ara::diag::MakeErrorCode(
      ara::diag::UdsDiagErrc::kSubFunctionNotSupported);
  }
  ara::core::Result<void> Write(
      const std::vector<uint8_t> &payload) noexcept override {
  ara::log::LogError() << "Receive write req";
    if (payload.size() != 1) {
      return ara::diag::MakeErrorCode(
        ara::diag::UdsDiagErrc::kInvalidMessageLengthFormat);
    }
    handler_->ChangePrimerState(payload[0]);
    return {};
  }

 public:
  PrimerDID(const ara::core::InstanceSpecifier& instance, std::shared_ptr<PrimerController> handler)
                        : ara::diag::GenericDiD{instance}, handler_(handler) {
      }
};

}  // namespace primer
}  // namespace srp

#endif  // APPS_EC_PRIMER_SERVICE_PRIMER_DID_HPP_
