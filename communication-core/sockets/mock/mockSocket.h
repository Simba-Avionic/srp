/**
 * @file mockSocket.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOCKETS_MOCK_MOCKSOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_MOCK_MOCKSOCKET_H_

#include <string>
#include <vector>
#include "gmock/gmock.h"

#include "communication-core/sockets/socket_config.h"
#include "communication-core/sockets/Isocket.h"
#include "communication-core/sockets/Isocket_stream.h"

class MockSocket : public srp::com::soc::ISocket {
 public:
  MOCK_METHOD(void, SetRXCallback, (srp::com::soc::RXCallback), (override));
  MOCK_METHOD(srp::core::ErrorCode, Init, (const srp::com::soc::SocketConfig&), (override));
  MOCK_METHOD(srp::core::ErrorCode, Transmit, (const std::string&, const std::uint16_t,
                                   std::vector<std::uint8_t>), (override));
  MOCK_METHOD(void, StartRXThread, (), (override));
};

class MockStreamSocket : public srp::srp::bindings::com::soc::ISocketStream {
 public:
  MOCK_METHOD(srp::core::ErrorCode, Init, (const srp::com::soc::SocketConfig& config), (override));
  MOCK_METHOD(void, SetRXCallback, (srp::com::soc::RXCallbackStream callback), (override));
  MOCK_METHOD(std::optional<std::vector<uint8_t>>, Transmit,
              (const std::string& ip, const std::uint16_t port,
               std::vector<std::uint8_t> payload),
              (override));
  MOCK_METHOD(void, StartRXThread, (), (override));
  MOCK_METHOD(void, StopRXThread, (), (override));
};
#endif  // COMMUNICATION_CORE_SOCKETS_MOCK_MOCKSOCKET_H_
