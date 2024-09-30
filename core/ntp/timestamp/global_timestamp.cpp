/**
 * @file global_timestamp.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/ntp/timestamp/global_timestamp.hpp"
#include <chrono>  // NOLINT
namespace simba {
namespace ntp {

namespace {
  constexpr auto kShm_name = "global_timestamp";
}  // namespace

GlobalTimestampController::GlobalTimestampController():shm_(ara::core::InstanceSpecifier{kShm_name}) {
  auto res = shm_.FindService();
}
GlobalTimestampController::~GlobalTimestampController() {
}
std::optional<uint64_t> GlobalTimestampController::GetTimestamp() {
  auto duration = GetNowInMS();

  auto val_opt = shm_.GetNewSamples();
  if (!val_opt.HasValue()) {
    return std::nullopt;
  }
  this->globalTimestamp_ = duration - val_opt.Value();
  return globalTimestamp_;
}

}  // namespace ntp
}  // namespace simba
