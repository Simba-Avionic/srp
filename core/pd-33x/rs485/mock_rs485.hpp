/**
 * @file mock_rs485.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CORE_PD_33X_RS485_MOCK_RS485_HPP_
#define CORE_PD_33X_RS485_MOCK_RS485_HPP_
#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <gmock/gmock.h>
#include "core/pd-33x/rs485/rs485.hpp"
#include "core/pd-33x/rs485/Irs485.hpp"
class MockRS485 : public srp::core::IRS485 {
 public:
    MOCK_METHOD(bool, Init,
        (const srp::core::RS485_conf_t& config,
        std::unique_ptr<srp::core::uart::IUartDriver>&& uart,
        std::unique_ptr<srp::gpio::IGPIOController>&& gpio),
        (override));

    MOCK_METHOD(std::optional<std::vector<uint8_t>>, WriteRead,
        (const std::vector<uint8_t>& data, uint8_t read_size),
        (override));

    ~MockRS485() override = default;
};

#endif  // CORE_PD_33X_RS485_MOCK_RS485_HPP_
