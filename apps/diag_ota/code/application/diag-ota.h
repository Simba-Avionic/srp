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

#include "communication-core/someip-controller/event_skeleton.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/application/application_no_ipc.h"
#include "diag/base/controller/idiag_controller.h"
namespace simba {
namespace diag_ota {

class DiagOta : public core::ApplicationNoIPC {
 protected:
  std::unique_ptr<diag::IDiagController> someip_diag_controller;
  uint32_t ecu_id{0};
  uint32_t eng_token{0};
  uint32_t plant_token{0};
  uint32_t mode{0};
  std::shared_ptr<com::someip::EventSkeleton> mode_event;
  std::shared_ptr<com::someip::MethodSkeleton> read_someip;
  std::shared_ptr<com::someip::MethodSkeleton> write_someip;
  std::shared_ptr<com::someip::MethodSkeleton> job_someip;
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
  std::optional<std::vector<uint8_t>> ReadSomeip(
      const std::vector<uint8_t> payload);
  std::optional<std::vector<uint8_t>> JobSomeip(
      const std::vector<uint8_t> payload);
  std::optional<std::vector<uint8_t>> WriteSomeip(
      const std::vector<uint8_t> payload);

 public:
  ~DiagOta() = default;
};

}  // namespace diag_ota
}  // namespace simba
#endif  // APPS_DIAG_OTA_CODE_APPLICATION_DIAG_OTA_H_
