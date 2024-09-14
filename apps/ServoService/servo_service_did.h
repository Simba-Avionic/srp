/**
 * @file example_did.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#define PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
#include <string>
#include <strstream>
#include "ara/log/log.h"
#include "diag/jobs/skeleton/did_job.h"
namespace simba {
namespace service {

class ServoServiceDiD : public diag::DiDJob {
 private:
  /**
   * @brief Callback for 0x22 UDS job (Read data by id) *optional
   *
   * @return DiagResponse
   */
  diag::DiagResponse Read() {
    ara::log::LogInfo() << "Działa";
    return diag::DiagResponse(diag::DiagResponseCodes::kOk,
                              {0x10, 0x11, 0x22, 0x33});
  }
diag::DiagResponse Write(const std::vector<uint8_t>& payload) {
  
  std::strstream buff{} ;
  for(const auto& v : payload){
    buff << static_cast<int>(v) <<",";
  }
 ara::log::LogInfo() << "("<<static_cast<uint8_t>(payload.size())<<") New status RX: " << buff.str();
  return diag::DiagResponse{diag::DiagResponseCodes::kOk};
}
 public:
  explicit ServoServiceDiD(const ara::core::InstanceSpecifier& instance)
      : diag::DiDJob(instance) {}
};

}  // namespace platform
}  // namespace simba

#endif  // PLATFORM_COMMON_DIAG_DEMON_CODE_SERVICES_EXAMPLE_DID_H_
