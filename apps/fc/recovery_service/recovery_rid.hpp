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
#include "apps/fc/recovery_service/recovery_rid.hpp"
#include <mutex>  // NOLINT
#include <thread>  // NOLINT
#include <vector>
#include <optional>
#include <memory>
#include <atomic>
#include <stop_token>  // NOLINT
#include "ara/diag/generic_routine.h"
#include "ara/diag/generic_data_identifier.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "apps/fc/recovery_service/parachute_controller.hpp"

namespace srp {
namespace apps {

class RecoveryGenericRoutine : public ara::diag::GenericRoutine {
 private:
  std::shared_ptr<apps::recovery::ParachuteController> controller;
  std::stop_token app_token;
 public:
  explicit RecoveryGenericRoutine(const ara::core::InstanceSpecifier &specifier);

  ara::core::Result<ara::diag::OperationOutput> Start(std::vector<uint8_t> requestData);

  ara::core::Result<ara::diag::OperationOutput> Stop(std::vector<uint8_t> requestData);

  ara::core::Result<ara::diag::OperationOutput> RequestResults(std::vector<uint8_t> requestData);
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_RECOVERY_RID_HPP_
