/**
 * @file ptp_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_NTP_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
#define CORE_NTP_NTP_CONTROLLER_NTP_CONTROLLER_HPP_

#include <string>
#include <memory>
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include "ara/com/shm/shm_skeleton.h"
#include "communication-core/sockets/udp_socket.h"
using kTime_t = uint64_t;

namespace simba {
namespace ntp {

class NTPController {
 private:
  ara::com::shm::ShmSkeleton<kTime_t> shm_;
  com::soc::UdpSocket sock_;
  const std::string kIp_file_path = "/opt/cpu_simba/logger_config.json";
  std::string master_;
  uint8_t id;
  uint64_t start_timestamp;
  std::unique_ptr<std::jthread> transmit_thread;
  uint64_t GetNowTimestamp();
  void CorrectStartPoint(const uint64_t& T1, const uint64_t& T2, const uint64_t& T3, const uint64_t& T4);
 public:
  NTPController();
  void transmit_loop(std::stop_token token);
  int Init(const std::string& master, const uint8_t& id);
  ~NTPController();
};

}  // namespace ntp
}  // namespace simba

#endif  // CORE_NTP_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
