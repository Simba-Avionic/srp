/**
 * @file diag_error_domain.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "ara/diag/diag_error_domain.h"

#include <unordered_map>
#include <string>

#include "ara/core/string_view.h"

namespace ara {
namespace diag {
namespace {
std::unordered_map<ara::core::ErrorDomain::CodeType, ara::core::StringView>
    kKeyList{
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kAlreadyOffered),
         "Already Offered"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kBusy),
         "Busy"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kConfigurationMismatch),
         "Configuration Mismatch"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kCustomResetTypeNotSupported),
         "Custom Reset Type Not Supported"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kDebouncingConfigurationInconsistent),
         "Debouncing Configuration Inconsistent"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kFailed),
         "Failed"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kInvalidArgument),
         "Invalid Argument"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kMemoryError),
         "Memory Error"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kNoSuchDTC),
         "No Such DTC"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kNotOffered),
         "Not Offered"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kRejected),
         "Rejected"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kReportIgnored),
         "Report Ignored"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kRequestFailed),
         "Request Failed"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kResetTypeNotSupported),
         "Reset Type Not Supported"},
        {static_cast<ara::core::ErrorDomain::CodeType>(
             DiagErrc::kSuppressionIgnored),
         "Suppression Ignored"},
        {static_cast<ara::core::ErrorDomain::CodeType>(DiagErrc::kWrongDtc),
         "Wrong Dtc"}};
const DiagErrorDomain domain_;
}  // namespace

DiagErrorDomain::DiagErrorDomain() noexcept
    : ara::core::ErrorDomain{0x8000000000000403} {}
const std::string DiagErrorDomain::Name() const noexcept {
  return std::string{"CoreErrorDomain"};
}
const std::string DiagErrorDomain::Message(
    ara::core::ErrorDomain::CodeType errorCode) const noexcept {
  return kKeyList[errorCode];
}

ara::core::ErrorCode MakeErrorCode(
    DiagErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetDiagDomain(),
      data};
}

ara::core::ErrorCode MakeErrorCode(
    DiagOfferErrc code, ara::core::ErrorDomain::SupportDataType data) noexcept {
  return ara::core::ErrorCode{
      static_cast<ara::core::ErrorDomain::CodeType>(code), GetDiagDomain(),
      data};
}

constexpr const ara::core::ErrorDomain& GetDiagDomain() noexcept {
  return domain_;
}
}  // namespace diag
}  // namespace ara
