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

#include <memory>
#include <vector>
#include <unordered_map>

#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/socket_config.h"
#include "core/logger/Logger.h"

#include "mw/temp/subscribe_msg/subscribe_header.h"
#include "mw/temp/subscribe_msg/subscribe_msg_factory.h"

#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"

#include "mw/temp/service/temp_service.h"

namespace simba {
namespace mw {
namespace temp {

class TempController {
 protected:
  uint16_t service_id;
  com::soc::IpcSocket sub_sock_{};
  std::vector<TempReading> latest_readings;
  simba::mw::temp::TempReadingMsgFactory factory;
  simba::com::soc::RXCallback callback_;

 private:
  virtual simba::core::ErrorCode Init(uint16_t service_id, simba::com::soc::RXCallback callback);

 public:
  virtual void SetTempRXCallback();
  virtual simba::core::ErrorCode Subscribe();

  void TempRXCallback(const std::string& ip,
     const std::uint16_t& port, const std::vector<std::uint8_t> data);
};

}  // namespace temp
}  // namespace mw
}  // namespace simba

#endif  // MW_TEMP_CONTROLLER_TEMP_CONTROLLER_H_
