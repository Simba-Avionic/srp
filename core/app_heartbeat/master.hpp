/**
 * @file master.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_APP_HEARTBEAT_MASTER_HPP_
#define CORE_APP_HEARTBEAT_MASTER_HPP_
#include <stop_token>
#include <thread>  // NOLINT
#include <chrono>  // NOLINT
#include <functional>
#include <mutex>
#include "communication-core/sockets/ipc_socket.h"
#include "core/app_heartbeat/common.hpp"

namespace srp {
namespace core {
namespace hb {
using timepoint = std::chrono::_V2::high_resolution_clock::time_point;

struct app_t {
    timepoint last_hb;
    bool active;
};

class HeartBeatMaster {
 private:
  using onUpdate = std::function<void(uint16_t)>;
  com::soc::IpcSocket sock_;
  onUpdate callback_;
  std::mutex mtx_;
  std::unordered_map<uint16_t, app_t> apps;

  std::jthread thread;
  void Loop(const std::stop_token& t);
  void RxCallback(const std::string& ip, const std::uint16_t& port,
                       std::vector<std::uint8_t> data);
 public:
  void Init(onUpdate callback);
  bool CheckAppAlive(const SERVICES_e& app);
  uint16_t GetAppsAlive();
};

}  // namespace hb
}  // namespace core
}  // namespace srp

#endif  // CORE_APP_HEARTBEAT_MASTER_HPP_
