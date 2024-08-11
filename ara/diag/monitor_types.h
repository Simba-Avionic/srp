/**
 * @file monitor_types.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef ARA_DIAG_MONITOR_TYPES_H_
#define ARA_DIAG_MONITOR_TYPES_H_
#include <cstdint>

namespace ara {
namespace diag {

enum class MonitorAction : std::uint32_t {
  kPassed = 0x00,
  kFailed = 0x01,
  kPrepassed = 0x02,
  kPrefailed = 0x03,
  kFdcThresholdReached = 0x04,
  kResetTestFailed = 0x05,
  kFreezeDebouncing = 0x06,
  kResetDebouncing = 0x07
};

enum class InitMonitorReason : std::uint32_t {
  kClear = 0x00,
  kRestart = 0x01,
  kReenabled = 0x02,
  kDisabled = 0x03
};

struct CounterBased {
  std::int16_t failed_threshold;
  std::int16_t passed_threshold;
  std::uint16_t failed_stepsize;
  std::uint16_t passed_stepsize;
  std::int16_t failed_jump_value;
  std::int16_t passed_jump_value;
  bool use_jump_to_failed;
  bool use_jump_to_passed;
};

struct TimeBased {
    std::uint32_t passed_ms;
    std::uint32_t failed_ms;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_MONITOR_TYPES_H_
