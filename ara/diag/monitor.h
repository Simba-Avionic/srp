/**
 * @file monitor.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ARA_DIAG_MONITOR_H_
#define ARA_DIAG_MONITOR_H_

#include <functional>
#include <optional>

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/diag/monitor_types.h"

namespace ara {
namespace diag {
class Monitor final {
 private:
  const ara::core::InstanceSpecifier instance_;
  std::optional<CounterBased> counter_based_;
  std::optional<TimeBased> time_base_;
  std::function<void(InitMonitorReason)> init_monitor_callback_;
  std::function<int8_t()> get_fault_detection_counter_callback_;

 public:
  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)> init_monitor,
          std::function<int8_t()> get_fault_detection_counter);

  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)> init_monitor,
          CounterBased default_values);

  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)> init_monitor,
          TimeBased default_values);
  Monitor(Monitor&&) noexcept = delete;
  Monitor(Monitor&) = delete;

  Monitor& operator=(Monitor&&) = delete;
  Monitor& operator=(Monitor&) = delete;

  void ReportMonitorAction(MonitorAction action);
  ara::core::Result<void> Offer();
  void StopOffer();
  ~Monitor();
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_MONITOR_H_
