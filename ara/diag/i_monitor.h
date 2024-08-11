#ifndef ARA_DIAG_I_MONITOR_H_
#define ARA_DIAG_I_MONITOR_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/diag/monitor_types.h"

namespace ara {
namespace diag {
class IMonitor {
 public:
  virtual void ReportMonitorAction(MonitorAction action) = 0;
  virtual ara::core::Result<void> Offer() = 0;
};
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_I_MONITOR_H_
