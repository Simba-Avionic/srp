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

  NtpConfig ntp_config_;
  bool is_holdover_ = true;

  int64_t last_t0_;
  int64_t last_sync_;

  DiscoveryManager discovery_manager_;
  core::timestamp::TimestampMaster timestamp_;

  std::jthread ntp_thread;

  void SendAnnounce();
  void SendSyncRequest(const std::string& current_master_ip);
  std::optional<srp::mw::tinyNTP::ntpStruct> ParseAndValidatePayload(
                        const std::vector<uint8_t>& payload, const std::string& ip);

 public:
  bool Init(const NtpConfig& config);

  void udp_socket_callback(const std::string& ip, const std::uint16_t& port,
                                                       const std::vector<std::uint8_t>& payload);
  void multicast_socket_callback(const std::string& ip, const std::uint16_t& port,
                                              const std::vector<std::uint8_t>& payload);

  void thread_loop(std::stop_token token);
  uint8_t EncodeSettings(const uint8_t device_class, const bool is_holdover, const uint8_t msg_type);
  int64_t CalculateOffset(const int64_t& T0, const int64_t& T1, const int64_t& T2, const int64_t& T3);
  uint64_t CalculateRoundTripDelay(const int64_t& T0, const int64_t& T1, const int64_t& T2, const int64_t& T3);
  int64_t GetTimestamp();
};

}  // namespace tinyNTP
}  // namespace srp

#endif  // MW_TIMESTAMP_MW_NTP_CONTROLLER_NTP_CONTROLLER_HPP_
