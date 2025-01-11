/**
 * @file common_connector.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "platform/common/someip_demon/code/connector/common_connector.h"

namespace srp {
namespace someip_demon {
namespace connectors {
void CommonConnector::RegisterSockets(
    std::shared_ptr<ara::com::ProccessSocket> ipc_soc,
    std::shared_ptr<common::soc::UdpSocket> udp_sock) {
  this->ipc_soc_ = ipc_soc;
  this->udp_sock_ = udp_sock;
}

}  // namespace connectors
}  // namespace someip_demon
}  // namespace srp
