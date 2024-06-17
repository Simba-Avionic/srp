/**
 * @file mock_udp_multicast_socket.h
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief a mock for udp_multicast_socket.h
 * @version 1.0
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOCKETS_MOCK_MOCK_UDP_MULTICAST_SOCKET_H_
#define COMMUNICATION_CORE_SOCKETS_MOCK_MOCK_UDP_MULTICAST_SOCKET_H_
#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "udp_multicast_socket.h"

class MockUDPMulticastSocket : public simba::com::soc::ISocket {
 public:
    MOCK_METHOD(simba::core::ErrorCode, Init, (const simba::com::soc::SocketConfig&), (override));
    MOCK_METHOD(void, SetRXCallback, (simba::com::soc::RXCallback), (override));
    MOCK_METHOD(simba::core::ErrorCode, Transmit, (const std::string&, const std::uint16_t,
                                        std::vector<std::uint8_t>), (override));
    MOCK_METHOD(void, StartRXThread, (), (override));
    MOCK_METHOD(void, Loop, (std::stop_token stoken), (override));
};
#endif  // COMMUNICATION_CORE_SOCKETS_MOCK_MOCK_UDP_MULTICAST_SOCKET_H_
