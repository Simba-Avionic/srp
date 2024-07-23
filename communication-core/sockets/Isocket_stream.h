/**
 * @file Isocket_stream.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOCKETS_ISOCKET_STREAM_H_
#define COMMUNICATION_CORE_SOCKETS_ISOCKET_STREAM_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <optional>

#include "communication-core/sockets/socket_config.h"
#include "core/common/error_code.h"
namespace simba {
namespace com {
namespace soc {
using RXCallbackStream =
    std::function<std::vector<uint8_t>(const std::string& ip, const std::uint16_t& port,
                       std::vector<std::uint8_t>)>;
class ISocketStream {
 public:
  /**
   * @brief Socket init function
   *
   * @param config Config file
   * @return core::ErrorCode initialiaze status
   */
  virtual simba::core::ErrorCode Init(const SocketConfig& config) = 0;
  /**
   * @brief Setter for rx callback
   *
   * @param callback
   */
  virtual void SetRXCallback(RXCallbackStream callback) = 0;
  /**
   * @brief Function to send data by socket
   *
   * @param ip target ip or path
   * @param port target port or 0 for ipcs
   * @param payload payload to send
   * @return core::ErrorCode status
   */
  virtual std::optional<std::vector<uint8_t>> Transmit(const std::string& ip,
                                   const std::uint16_t port,
                                   std::vector<std::uint8_t> payload) = 0;
  /**
   * @brief This function start RX thread
   *
   */
  virtual void StartRXThread() = 0;
  virtual ~ISocketStream() {}
};

}  // namespace soc
}  // namespace com
}  // namespace simba


#endif  // COMMUNICATION_CORE_SOCKETS_ISOCKET_STREAM_H_
