/**
 * @file diag-ota.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef APPS_DIAG_OTA_CODE_APPLICATION_DIAG_OTA_H_
#define APPS_DIAG_OTA_CODE_APPLICATION_DIAG_OTA_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "apps/diag_ota/code/application/services/uds_server.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "core/application/application_no_ipc.h"
#include "diag/jobs/skeleton/did_job.h"

namespace simba {
namespace diag_ota {

class DiagOta : public core::ApplicationNoIPC {
 protected:
  uint32_t ecu_id{0};
  uint32_t eng_token{0};
  uint32_t plant_token{0};
  uint32_t mode{0};
  std::shared_ptr<com::someip::EventSkeleton> mode_event;
  std::unique_ptr<uds::UdsServer> uds_server;
  std::unique_ptr<diag::DiDJob> read_vin_;
  std::unique_ptr<diag::DiDJob> uptime_;
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

 public:
  ~DiagOta() = default;
};

}  // namespace diag_ota
}  // namespace simba
#endif  // APPS_DIAG_OTA_CODE_APPLICATION_DIAG_OTA_H_
