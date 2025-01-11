/**
 * @file recovery_rid.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef APPS_FC_RECOVERY_SERVICE_RECOVERY_RID_HPP_
#define APPS_FC_RECOVERY_SERVICE_RECOVERY_RID_HPP_
#include <vector>
#include <memory>
#include "ara/diag/generic_routine.h"
#include "ara/diag/generic_data_identifier.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "apps/fc/recovery_service/parachute_controller.hpp"
namespace srp {
namespace apps {
enum STATE_T:uint8_t {
  NON_OPERATE = 0,
  PARACHUTE_OPENING_SEQ = 1,
  PARACHUTE_UNREEF_SEQ = 2,
};
class RecoveryGenericRoutine : public ara::diag::GenericRoutine {
 private:
  const std::shared_ptr<apps::recovery::ParachuteController> controller;
  STATE_T state;

 public:
  RecoveryGenericRoutine(const ara::core::InstanceSpecifier &specifier,
                      const std::shared_ptr<recovery::ParachuteController>& contr):
                      GenericRoutine{specifier}, controller(contr), state(STATE_T::NON_OPERATE) {
  }
  ara::core::Result<ara::diag::OperationOutput> Start(std::vector<uint8_t> requestData) {
    if (requestData.size() != 1) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "invalid payload");
    }
    if (requestData[0] == 0) {
        state = STATE_T::PARACHUTE_OPENING_SEQ;
        controller->OpenParachute(true);
        state = STATE_T::NON_OPERATE;
        return ara::diag::OperationOutput{{1}};
    } else if (requestData[0] == 1) {
        state = STATE_T::PARACHUTE_UNREEF_SEQ;
        controller->UnreefParachute(true);
        state = STATE_T::NON_OPERATE;
        return ara::diag::OperationOutput{{1}};
    } else if (requestData[0] == 2) {
        state = STATE_T::PARACHUTE_OPENING_SEQ;
        controller->OpenParachute(true);
        state = STATE_T::PARACHUTE_UNREEF_SEQ;
        controller->UnreefParachute(true);
        state = STATE_T::NON_OPERATE;
        return ara::diag::OperationOutput{{1}};
    }
    state = STATE_T::NON_OPERATE;
    return ara::diag::OperationOutput{};
  }

  ara::core::Result<ara::diag::OperationOutput> Stop(std::vector<uint8_t> requestData) {
    return ara::com::MakeErrorCode(
            ara::com::ComErrc::kSetHandlerNotSet, "handler not set function not implemented");
  }

  ara::core::Result<ara::diag::OperationOutput> RequestResults(std::vector<uint8_t> requestData)  {
    return  ara::diag::OperationOutput{{state}};
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_RECOVERY_RID_HPP_
