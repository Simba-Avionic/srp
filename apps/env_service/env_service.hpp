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
#ifndef APPS_ENV_SERVICE_ENV_SERVICE_HPP_
#define APPS_ENV_SERVICE_ENV_SERVICE_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "mw/temp/controller/temp_controller.h"
#include "core/application/application_no_ipc.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "diag/dtc/controller/dtc.h"

namespace simba {
namespace envService {

class EnvService : public core::ApplicationNoIPC{
 private:
  std::shared_ptr<simba::diag::dtc::DTCObject> dtc_temp_error;
  std::shared_ptr<diag::dtc::DTCObject> dtc_temp_connection_error_0xB0;
  mw::temp::TempController temp_{};
  std::shared_ptr<com::someip::EventSkeleton> temp1_event;
  std::shared_ptr<com::someip::EventSkeleton> temp2_event;
  std::shared_ptr<com::someip::EventSkeleton> temp3_event;

 protected:
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialiaze the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
  void TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data);
  bool CheckTempError(const double &value);

 public:
  ~EnvService() = default;
};

}  // namespace envService
}  // namespace simba


#endif  // APPS_ENV_SERVICE_ENV_SERVICE_HPP_
