/**
 * @file system_stat_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-02-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef APPS_EC_SYSTEM_STAT_SERVICE_SYSTEM_STAT_SERVICE_HPP_
#define APPS_EC_SYSTEM_STAT_SERVICE_SYSTEM_STAT_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include "ara/exec/adaptive_application.h"
#include "srp/apps/SysStatServiceSkeleton.h"

namespace srp {
namespace sysService {

class SystemStatService final : public ara::exec::AdaptiveApplication {
 private:
  apps::SysStatServiceSkeleton service_ipc;
  apps::SysStatServiceSkeleton service_udp;

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
 public:
  ~SystemStatService() = default;
  SystemStatService();
};

}  // namespace sysService
}  // namespace srp


#endif  // APPS_EC_SYSTEM_STAT_SERVICE_SYSTEM_STAT_SERVICE_HPP_
