/**
 * @file temp_controller.h
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_
#define MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_

#include <string>
#include <cstdio>
#include <cinttypes>
#include <cstdint>
#include <utility>

#include <memory>
#include <vector>
#include <unordered_map>

#include "communication-core/sockets/Isocket.h"
#include "communication-core/sockets/socket_config.h"
#include "ara/log/log.h"
#include "mw/temp/service/temp_service.h"
#include "srp/mw/temp/TempReadHdr.h"
#include "srp/mw/temp/TempSubHdr.h"

namespace srp {
namespace mw {
namespace temp {

using TempRXCallback =
std::function<void(const std::vector<srp::mw::temp::TempReadHdr>&)>;

/**
 * @brief Function to receive temp readings from 1Wire using TEMP_MW
 * 
 */
class TempController {
 private:
  uint16_t service_id;
  std::unique_ptr<com::soc::ISocket> sub_sock_{};
  TempRXCallback callback_;

 protected:
  void SetTempRXCallback();
  srp::core::ErrorCode Init(uint16_t service_id, std::unique_ptr<com::soc::ISocket> sock);
  srp::core::ErrorCode SetUp(TempRXCallback callback);
  std::vector<srp::mw::temp::TempReadHdr> Conv(const std::vector<uint8_t>& data) const;
 public:
 /**
  * @brief Initialize function for temp receive
  * 
  * @param service_id 
  * @param callback 
  * @param sock 
  * @return srp::core::ErrorCode 
  */
  srp::core::ErrorCode Initialize(uint16_t service_id, TempRXCallback callback, std::unique_ptr<com::soc::ISocket> sock);
  srp::core::ErrorCode Subscribe(std::string name);
  srp::core::ErrorCode Register(std::string name);
};

}  // namespace temp
}  // namespace mw
}  // namespace srp

#endif  // MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_
