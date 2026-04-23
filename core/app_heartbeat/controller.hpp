/**
 * @file controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_APP_HEARTBEAT_CONTROLLER_HPP_
#define CORE_APP_HEARTBEAT_CONTROLLER_HPP_
#include <stop_token>
#include <thread>
#include "communication-core/sockets/ipc_socket.h"
#include "core/app_heartbeat/common.hpp"

namespace srp {
namespace core {
namespace hb {

struct hb_config_t {

};

class HeartBeatController {
 private:
  com::soc::IpcSocket sock_;
  uint16_t id_;
  std::jthread thread;
  void Loop(const std::stop_token& t);
 public:
  HeartBeatController();
  void Init(const SERVICES_e id);
};

}  // namespace hb
}  // namespace core
}  // namespace srp

#endif  // CORE_APP_HEARTBEAT_CONTROLLER_HPP_
