/**
 * @file ntp_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MW_TIMESTAMP_MW_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
#define MW_TIMESTAMP_MW_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <thread>  // NOLINT
#include "communication-core/sockets/udp_socket.h"
#include "communication-core/sockets/udp_multicast_socket.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "srp/mw/tinyNTP/ntpStruct.h"
#include "mw/timestamp_mw/ntp/config/config_manager.hpp"
#include "mw/timestamp_mw/ntp/discovery/discovery_manager.hpp"
namespace srp {
namespace tinyNTP {

class NtpController {
 private:
  com::soc::UdpSocket udp_sock_;
  com::soc::UdpMulticastSocket multicast_sock_;

  core::timestamp::TimestampMaster timestamp_;
  std::jthread ntp_thread;
  std::string myIP;
  uint8_t ntp_class_;
  uint32_t t_hb_ms_;
  int64_t last_t0_;
  DiscoveryManager discovery_manager_;

  void SendAnnounce();
  uint8_t EncodeSettings(uint8_t device_class, bool is_holdover, uint8_t msg_type);
  void SendSyncRequest(const std::string& current_master_ip);
 public:
  bool Init(const NtpConfig& config);

  void socket_callback(const std::string& ip, const std::uint16_t& port,
                                                       const std::vector<std::uint8_t>& payload);                                              
                                                      
  void thread_loop(std::stop_token token);
  int64_t CalculateOffset(const int64_t T0, const int64_t T1, const int64_t T2, const int64_t T3);
  uint64_t CalculateRoundTripDelay(const int64_t T0, const int64_t T1, const int64_t T2, const int64_t T3);
  int64_t GetTimestamp();
};

}  // namespace tinyNTP
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
