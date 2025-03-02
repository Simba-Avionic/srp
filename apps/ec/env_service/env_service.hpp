/**
 * @file env_service.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_EC_ENV_SERVICE_ENV_SERVICE_HPP_
#define APPS_EC_ENV_SERVICE_ENV_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

#include "mw/temp/controller/temp_controller.h"
#include "ara/exec/adaptive_application.h"
#include "srp/env/EnvAppSkeleton.h"
#include "communication-core/sockets/ipc_socket.h"

namespace srp {
namespace envService {

class EnvService final : public ara::exec::AdaptiveApplication {
 private:
  env::EnvAppSkeleton service_ipc;
  env::EnvAppSkeleton service_udp;
  std::unique_ptr<mw::temp::TempController> temp_{};

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  int Run(const std::stop_token& token) override;
  core::ErrorCode Init(std::unique_ptr<mw::temp::TempController> temp);
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  int Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) override;
  void TempRxCallback(const std::vector<srp::mw::temp::TempReadHdr>& data);

 public:
  ~EnvService() = default;
  EnvService();
};

}  // namespace envService
}  // namespace srp


#endif  // APPS_EC_ENV_SERVICE_ENV_SERVICE_HPP_
