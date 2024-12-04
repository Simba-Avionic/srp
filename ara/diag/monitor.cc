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

#include <utility>

#include "ara/core/model/diag_model_item.h"
#include "ara/core/model_db.h"

namespace ara {
namespace diag {
Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)>&& init_monitor,
                 std::function<int8_t()>&& get_fault_detection_counter)
    : instance_{instance},
      init_monitor_callback_{std::move(init_monitor)},
      get_fault_detection_counter_callback_{
          std::move(get_fault_detection_counter)},
      menager_{DiagnosticMenager::GetInstance()} {}

Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)>&& init_monitor,
                 CounterBased default_values)
    : instance_{instance},
      init_monitor_callback_{std::move(init_monitor)},
      menager_{DiagnosticMenager::GetInstance()} {}

Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)>&& init_monitor,
                 TimeBased default_values)
    : instance_{instance},
      init_monitor_callback_{std::move(init_monitor)},
      menager_{DiagnosticMenager::GetInstance()} {}
Monitor::Monitor(const ara::core::InstanceSpecifier& instance,
                 std::function<void(InitMonitorReason)>&& init_monitor)
    : instance_{instance},
      init_monitor_callback_{std::move(init_monitor)},
      menager_{DiagnosticMenager::GetInstance()} {}

void Monitor::ReportMonitorAction(MonitorAction action) {
  switch (action) {
    case MonitorAction::kFailed:
    case MonitorAction::kPassed:
      menager_->SendDtcUpdate(id_, static_cast<uint8_t>(action));
      break;

    default:
      break;
  }
}
ara::core::Result<void> Monitor::Offer() {
  const auto model =
      ara::core::ModelDataBase::GetInstance()
          .ResolveInstanceSpecifier<ara::core::model::ModelDtc>(instance_);
  if (!model.HasValue()) {
    return model.Error();
  }
  const auto& model_v = model.Value();
  id_ = model_v.id_;
  name = model_v.name_;

  this->menager_->RegisterDtcHandler(id_, [](uint8_t new_status) {});

  return {};
}
void Monitor::StopOffer() {}
Monitor::~Monitor() {}

}  // namespace diag

}  // namespace ara
