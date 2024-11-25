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
#include "core/ptp/timestamp/global_timestamp.hpp"
#include <chrono>  // NOLINT
namespace simba {
namespace ptp {

namespace {
  constexpr auto kShm_name = "global_timestamp";
}  // namespace

GlobalTimestampController::GlobalTimestampController():shm_(ara::core::InstanceSpecifier{kShm_name}) {
  auto res = shm_.FindService();
}
GlobalTimestampController::~GlobalTimestampController() {
}
std::optional<uint64_t> GlobalTimestampController::GetTimestamp() {
  auto now = std::chrono::high_resolution_clock::now();
  auto val_opt = shm_.GetNewSamples();
  if (!val_opt.HasValue()) {
    return std::nullopt;
  }

  return std::chrono::duration(now - val_opt.Value()).count();
}

}  // namespace ptp
}  // namespace simba
