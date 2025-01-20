/**
 * @file primer_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_PRIMER_SERVICE_APP_HPP_
#define APPS_EC_PRIMER_SERVICE_APP_HPP_

#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>

#include "ara/exec/adaptive_application.h"
#include "apps/ec/primer_service/service.hpp"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
namespace srp {
namespace primer {

class PrimerService final :  public ara::exec::AdaptiveApplication {
 private:
  std::shared_ptr<primer::PrimerController> controller;
  apps::MyPrimerServiceSkeleton service_ipc;
  apps::MyPrimerServiceSkeleton service_udp;


  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;

 public:
  ~PrimerService() = default;
  PrimerService();
};

}  // namespace primer
}  // namespace srp
#endif  // APPS_EC_PRIMER_SERVICE_APP_HPP_
