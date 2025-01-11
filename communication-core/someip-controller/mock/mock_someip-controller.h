/**
 * @file mock_someip-controller.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COMMUNICATION_CORE_SOMEIP_CONTROLLER_MOCK_MOCK_SOMEIP_CONTROLLER_H_
#define COMMUNICATION_CORE_SOMEIP_CONTROLLER_MOCK_MOCK_SOMEIP_CONTROLLER_H_

#include <cstdint>
#include <vector>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "communication-core/someip-controller/Isomeip_controller.h"


class MockSomeIpController : public srp::com::someip::ISomeIpController{
 public:
  MOCK_METHOD((std::optional<std::vector<uint8_t>>), Request,
          (const uint16_t, const uint16_t, const std::vector<uint8_t>), (override));
  MOCK_METHOD((bool), RequestNoResponse, (const uint16_t, const uint16_t,
                                          const std::vector<uint8_t>), (override));
  MOCK_METHOD((srp::core::ErrorCode), AddMethod, (const uint16_t, srp::com::someip::SomeIPMethod), (override));
  MOCK_METHOD((srp::core::ErrorCode), AddEventValue, (const uint16_t, const std::vector<uint8_t>), (override));
  MOCK_METHOD((srp::core::ErrorCode), Init, (), (override));
  MOCK_METHOD((srp::core::ErrorCode), LoadServiceList, (const std::string&), (override));

  virtual ~MockSomeIpController() = default;
};

#endif  // COMMUNICATION_CORE_SOMEIP_CONTROLLER_MOCK_MOCK_SOMEIP_CONTROLLER_H_
