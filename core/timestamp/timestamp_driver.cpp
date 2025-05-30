/**
 * @file timestamp_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "core/timestamp/timestamp_driver.hpp"
#include "ara/log/log.h"

namespace srp {
namespace core {
namespace timestamp {

namespace {
  constexpr auto kTimestamp_instance = "TIMESTAMP";
  constexpr auto kMax_Wait = 5;
}
int64_t GetNow() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch())
    .count();
}

bool TimestampController::Init() {
  uint8_t i = 0;
  do {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    i += 1;
  } while (!proxy_.FindService().HasValue() && i <= kMax_Wait);
  // error
  if (i == kMax_Wait) {
    return false;
  }
  return true;
}
TimestampController::TimestampController(): instance_(kTimestamp_instance), proxy_(this->instance_) {
}
int64_t TimestampController::GetDeltaTime(const int64_t now, const int64_t previous) {
  return now - previous;
}

std::optional<int64_t> TimestampController::GetNewTimeStamp() {
    auto res = proxy_.GetNewSamples();
    if (!res.HasValue()) {
      return std::nullopt;
    }
    return GetNow() - res.Value();
}

TimestampMaster::TimestampMaster(): instance_(kTimestamp_instance), skeleton_(instance_) {
}

bool TimestampMaster::Init() {
  if (!skeleton_.OfferService().HasValue()) {
    return false;
  }
  start = GetNow();
  skeleton_.Send(start);
  return true;
}

void TimestampMaster::CorrectStartPoint(const int64_t offset) {
  start -= offset;
  skeleton_.Send(start);
}

int64_t TimestampMaster::GetNewTimeStamp() {
  return GetNow() - start;
}


}  // namespace timestamp
}  // namespace core
}  // namespace srp
