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
#include <memory>
#include <optional>
#include <string>
#include <thread>  // NOLINT

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/diag/diagnostic_menager.h"
#include "ara/diag/monitor_types.h"
namespace ara {
namespace diag {
class Monitor final {
 private:
  uint32_t id_{0x0U};
  std::string name{};
  std::unique_ptr<std::jthread> offer_loop_;
  const ara::core::InstanceSpecifier instance_;
  std::optional<CounterBased> counter_based_;
  std::optional<TimeBased> time_base_;
  std::function<void(InitMonitorReason)> init_monitor_callback_;
  std::function<int8_t()> get_fault_detection_counter_callback_;
  const std::shared_ptr<DiagnosticMenager> menager_;

 public:
  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)>&& init_monitor,
          std::function<int8_t()>&& get_fault_detection_counter);

  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)>&& init_monitor,
          CounterBased default_values);

  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)>&& init_monitor,
          TimeBased default_values);
  Monitor(const ara::core::InstanceSpecifier& instance,
          std::function<void(InitMonitorReason)>&& init_monitor);
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
