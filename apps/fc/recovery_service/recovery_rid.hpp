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
#include <memory>
#include <optional>
#include <atomic>
#include <stop_token>  // NOLINT
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
  WAITING = 3,  // Operacja w toku
  COMPLETED = 4,  // Operacja zakończona pomyślnie
  FAILED = 5,  // Operacja zakończona błędem
  STOPPED = 6,  // Operacja zatrzymana
};

constexpr auto kOPEN_PARACHUTE = 0;
constexpr auto kUNREEF_PARACHUTE = 1;


class RecoveryGenericRoutine : public ara::diag::GenericRoutine {
 private:
  const std::shared_ptr<apps::recovery::ParachuteController> controller;
  STATE_T state_;
  std::mutex state_mutex_;
  std::unique_ptr<std::jthread> operation_thread_;
  std::optional<bool> operation_result_;
  std::atomic<bool> operation_in_progress_;

  void ExecuteOpenParachute(std::stop_token token) {
    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      state_ = STATE_T::PARACHUTE_OPENING_SEQ;
      operation_result_ = std::nullopt;
    }

    bool result = false;
    if (!token.stop_requested()) {
      result = controller->OpenParachute(token, true);
    }

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      if (token.stop_requested()) {
        state_ = STATE_T::STOPPED;
        operation_result_ = false;
      } else if (result) {
        state_ = STATE_T::COMPLETED;
        operation_result_ = true;
      } else {
        state_ = STATE_T::FAILED;
        operation_result_ = false;
      }
      operation_in_progress_ = false;
    }
  }

  void ExecuteUnreefParachute(std::stop_token token) {
    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      state_ = STATE_T::PARACHUTE_UNREEF_SEQ;
      operation_result_ = std::nullopt;
    }

    bool result = false;
    if (!token.stop_requested()) {
      result = controller->UnreefParachute(token, true);
    }

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      if (token.stop_requested()) {
        state_ = STATE_T::STOPPED;
        operation_result_ = false;
      } else if (result) {
        state_ = STATE_T::COMPLETED;
        operation_result_ = true;
      } else {
        state_ = STATE_T::FAILED;
        operation_result_ = false;
      }
      operation_in_progress_ = false;
    }
  }

 public:
  RecoveryGenericRoutine(const ara::core::InstanceSpecifier &specifier,
                      const std::shared_ptr<recovery::ParachuteController>& contr):
                      GenericRoutine{specifier}, controller(contr), state_(STATE_T::NON_OPERATE),
                      operation_in_progress_(false) {
  }

  ~RecoveryGenericRoutine() {
    // Zatrzymaj wątek jeśli działa
    if (operation_thread_ && operation_thread_->joinable()) {
      operation_thread_->request_stop();
      operation_thread_->join();
    }
  }

  ara::core::Result<ara::diag::OperationOutput> Start(std::vector<uint8_t> requestData) {
    if (requestData.size() != 1) {
        return ara::com::MakeErrorCode(
            ara::com::ComErrc::kWrongMethodCallProcessingMode, "invalid payload");
    }

    // Zatrzymaj poprzednią operację jeśli działa
    if (operation_thread_ && operation_thread_->joinable()) {
      operation_thread_->request_stop();
      operation_thread_->join();
    }

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      operation_in_progress_ = true;
    }

    if (requestData[0] == kOPEN_PARACHUTE) {
        operation_thread_ = std::make_unique<std::jthread>(
            [this](std::stop_token token) {
                ExecuteOpenParachute(token);
            });
        // Zwróć OK natychmiast
        return ara::diag::OperationOutput{{1}};
    } else if (requestData[0] == kUNREEF_PARACHUTE) {
        operation_thread_ = std::make_unique<std::jthread>(
            [this](std::stop_token token) {
                ExecuteUnreefParachute(token);
            });
        // Zwróć OK natychmiast
        return ara::diag::OperationOutput{{1}};
    }

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      state_ = STATE_T::NON_OPERATE;
      operation_in_progress_ = false;
    }
    return ara::diag::OperationOutput{};
  }

  ara::core::Result<ara::diag::OperationOutput> Stop(std::vector<uint8_t> requestData) {
    // Siłowo zatrzymaj wątek
    if (operation_thread_ && operation_thread_->joinable()) {
      operation_thread_->request_stop();
      operation_thread_->join();
    }

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      if (operation_in_progress_) {
        state_ = STATE_T::STOPPED;
        operation_result_ = false;
        operation_in_progress_ = false;
      } else {
        state_ = STATE_T::NON_OPERATE;
      }
    }

    return ara::diag::OperationOutput{{1}};
  }

  ara::core::Result<ara::diag::OperationOutput> RequestResults(std::vector<uint8_t> requestData) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    // Jeśli operacja w toku, zwróć WAITING
    if (operation_in_progress_ && !operation_result_.has_value()) {
      return ara::diag::OperationOutput{{STATE_T::WAITING}};
    }

    // Zwróć aktualny stan (COMPLETED, FAILED, STOPPED, lub NON_OPERATE)
    return ara::diag::OperationOutput{{state_}};
  }
};

}  // namespace apps
}  // namespace srp

#endif  // APPS_FC_RECOVERY_SERVICE_RECOVERY_RID_HPP_
