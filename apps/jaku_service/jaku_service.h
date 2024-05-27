/**
 * @file jaku_service.h
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief sends temp data and diode requests to mima service
 * @version 1.0
 * @date 2024-05-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef APPS_JAKU_SERVICE_JAKU_SERVICE_H_
#define APPS_JAKU_SERVICE_JAKU_SERVICE_H_

#include "mw/temp/controller/temp_controller.h"
#include "core/application/application_no_ipc.h"
#include "communication-core/someip-controller/event_skeleton.h"
#include "diag/dtc/controller/dtc.h"

namespace simba {
namespace jakuService {

class JakuService : public core::ApplicationNoIPC {
 private:
    mw::temp::TempController temp_{};
    std::shared_ptr<com::someip::EventSkeleton> temp_event_1;
    std::shared_ptr<com::someip::EventSkeleton> temp_event_2;
    std::shared_ptr<com::someip::EventSkeleton> temp_event_3;
    std::vector<uint8_t> temp_vector_1;
    std::vector<uint8_t> temp_vector_2;
    std::vector<uint8_t> temp_vector_3;

    /* data */
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
  std::optional<std::vector<uint8_t>> WriteSomeip(
      const std::vector<uint8_t> payload);
  bool CheckTempError(const double &value);
 
 public:
  ~JakuService() = default;
};

}  // namespace jakuService
}  // namespace simba


#endif  // APPS_JAKU_SERVICE_JAKU_SERVICE_H_