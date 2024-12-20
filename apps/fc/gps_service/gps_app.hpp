/**
 * @file gps_app.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_FC_GPS_SERVICE_GPS_APP_HPP_
#define APPS_FC_GPS_SERVICE_GPS_APP_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>
#include <mutex>  // NOLINT

#include "ara/exec/adaptive_application.h"
#include "simba/apps/GPSServiceSkeleton.h"

#include "core/uart/uart_driver.hpp"
#include "core/gps/nmea.hpp"

namespace simba {
namespace apps {

class GPSApp final : public ara::exec::AdaptiveApplication {
 private:
  const ara::core::InstanceSpecifier service_ipc_instance;
  const ara::core::InstanceSpecifier service_udp_instance;
  std::unique_ptr<apps::GPSServiceSkeleton> service_ipc;
  std::unique_ptr<apps::GPSServiceSkeleton> service_udp;
  std::unique_ptr<core::uart::IUartDriver> uart_;

 public:
  void ParseGPSData(const std::vector<char>& data);
  void Init(std::unique_ptr<core::uart::IUartDriver> uart);
  static GPSDataStructure GetSomeIPData(const core::GPS_DATA_T& data);
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
  ~GPSApp();
  GPSApp();
};

}  // namespace apps
}  // namespace simba


#endif  // APPS_FC_GPS_SERVICE_GPS_APP_HPP_
