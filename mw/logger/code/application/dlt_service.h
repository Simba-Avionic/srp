/**
 * @file dlt_service.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MW_LOGGER_CODE_APPLICATION_DLT_SERVICE_H_
#define MW_LOGGER_CODE_APPLICATION_DLT_SERVICE_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "communication-core/sockets/ipc_socket.h"
#include "communication-core/sockets/udp_multicast_socket.h"
#include "core/application/application_mw.h"
#include "core/common/wait_queue.h"
#include "mw/logger/code/data/idlt_frame.h"

namespace simba {
namespace mw {
namespace dlt {
class DltService final : public core::ApplicationMW {
 private:
  com::soc::UdpMulticastSocket soc{};
  com::soc::IpcSocket ipc_soc{};
  std::string ip_;
  uint16_t tx_port;
  std::string ec_name;

  core::WaitQueue<std::shared_ptr<simba::dlt::data::IDLTFrame>, 200> logs{};
  /**
   * @brief This function is called to launch the application
   *
   * @param token stop token
   */
  core::ErrorCode Run(std::stop_token token) final;
  /**
   * @brief This function is called to initialize the application
   *
   * @param parms map with parms
   */
  core::ErrorCode Initialize(
      const std::unordered_map<std::string, std::string>& parms) final;
  void InitIPC() noexcept;
  void InitUDP() noexcept;

 public:
  DltService(/* args */);
  ~DltService();
};
}  // namespace dlt
}  // namespace mw
}  // namespace simba
#endif  // MW_LOGGER_CODE_APPLICATION_DLT_SERVICE_H_
