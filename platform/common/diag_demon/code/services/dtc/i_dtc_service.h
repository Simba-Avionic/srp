/**
 * @file i_dtc_service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_DTC_I_DTC_SERVICE_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_DTC_I_DTC_SERVICE_H_

namespace srp {
namespace platform {
namespace diag_demon {
namespace dtc {
class IDtcService {
 public:
  virtual void Start() noexcept = 0;
  virtual void Stop() noexcept = 0;
  virtual ~IDtcService() = default;
};
}  // namespace dtc
}  // namespace diag_demon
}  // namespace platform
}  // namespace srp

#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_DTC_I_DTC_SERVICE_H_
