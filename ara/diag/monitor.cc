/**
 * @file monitor.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ara/diag/monitor.h"

namespace ara {
namespace diag {
Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)> init_monitor,
                 std::function<int8_t()> get_fault_detection_counter)
    : instance_{instance},
      init_monitor_callback_{init_monitor},
      get_fault_detection_counter_callback_{get_fault_detection_counter} {}

Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)> init_monitor,
                 CounterBased default_values)
    : instance_{instance} {}

Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)> init_monitor,
                 TimeBased default_values)
    : instance_{instance} {}

void Monitor::ReportMonitorAction(MonitorAction action) {}
ara::core::Result<void> Monitor::Offer() { return {}; }
void Monitor::StopOffer() {}
Monitor::~Monitor() {}

}  // namespace diag

}  // namespace ara
