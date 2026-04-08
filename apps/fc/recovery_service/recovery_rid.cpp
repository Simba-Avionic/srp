/**
 * @file recovery_rid.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "apps/fc/recovery_service/recovery_rid.hpp"

namespace srp {
namespace apps {

constexpr auto kOPEN_PARACHUTE = 0;
constexpr auto kUNREEF_PARACHUTE = 1;

RecoveryGenericRoutine::RecoveryGenericRoutine(const ara::core::InstanceSpecifier &specifier):
                    GenericRoutine{specifier} {
  controller = apps::recovery::ParachuteController::GetInstance();
}

ara::core::Result<ara::diag::OperationOutput> RecoveryGenericRoutine::Start(std::vector<uint8_t> requestData) {
  if (requestData.size() != 1) {
      return ara::com::MakeErrorCode(
          ara::com::ComErrc::kWrongMethodCallProcessingMode, "invalid payload");
  }
  if (!(requestData[0] == kOPEN_PARACHUTE | requestData[0] == kUNREEF_PARACHUTE)) {
    return ara::com::MakeErrorCode(
          ara::com::ComErrc::kWrongMethodCallProcessingMode, "invalid payload");
  }
  switch (requestData[0]) {
    case kOPEN_PARACHUTE:
      controller->OpenParachute(app_token);
      break;
    case kUNREEF_PARACHUTE:
      controller->UnreefParachute(app_token);
      break;
    default:
      break;
  }
  return ara::diag::OperationOutput{{1}};
}


ara::core::Result<ara::diag::OperationOutput> RecoveryGenericRoutine::Stop(std::vector<uint8_t> requestData) {
  controller->StopThred();
  return ara::diag::OperationOutput{{1}};
}

ara::core::Result<ara::diag::OperationOutput> RecoveryGenericRoutine::RequestResults(std::vector<uint8_t> requestData) {
  return ara::diag::OperationOutput{{static_cast<uint8_t>(controller->GetParachuteState())}};
}


}  // namespace apps
}  // namespace srp
