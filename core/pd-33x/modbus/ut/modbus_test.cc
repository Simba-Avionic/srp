#include <gtest/gtest.h>
#include <iostream>
#include "core/pd-33x/modbus/modbus.hpp"
#include "core/pd-33x/rs485/mock_rs485.hpp"


class ModbusTestClass : srp::core::MODBUS {
 public:
 uint16_t TestCalculate_CRC16(const std::vector<uint8_t>& data) {
    return Calculate_CRC16(data);
 }
 bool TestValidate_CRC16(const std::vector<uint8_t>& data) {
    return Validate_CRC16(data);
 }
 bool TestInit(const srp::core::RS485_conf_t& config, const uint8_t slave_id,
    std::unique_ptr<MockRS485> rs485) {
        return Init(config, slave_id, std::move(rs485_));
    }
std::optional<std::vector<uint8_t>> TestSendRequest(uint8_t function_code, uint16_t start_addr, uint16_t quantity) {
    return SendRequest(function_code, start_addr, quantity);
}
};

class ModbusTest : public ::testing::Test {
 public:
    ModbusTestClass modbus;
};

class CalculateCRC16Test : public ModbusTest, public ::testing::WithParamInterface<std::pair<std::vector<uint8_t>, uint16_t>> {};

TEST_P(CalculateCRC16Test, CalculateCRC16)
{
    auto [data, expected_crc] = GetParam();
    EXPECT_EQ(modbus.TestCalculate_CRC16(data), expected_crc);
}

INSTANTIATE_TEST_SUITE_P(
    ModbusTests,
    CalculateCRC16Test,
    ::testing::Values(
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x00, 0x02, 0x00, 0x02}, 0xCB65),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x04, 0x3F, 0x75, 0xF0, 0x7B}, 0xDEE3),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x08, 0x3F, 0x75, 0xE3, 0xD2, 0x41, 0xB6, 0x1C, 0x20}, 0xC7A0)
    )
);

class ValidateCRC16Test : public ModbusTest, public ::testing::WithParamInterface<std::pair<std::vector<uint8_t>, bool>> {};

TEST_P(ValidateCRC16Test, ValidateCRC16)
{
    auto [data, is_valid] = GetParam();
    EXPECT_EQ(modbus.TestValidate_CRC16(data), is_valid);
}

INSTANTIATE_TEST_SUITE_P(
    ModbusTests,
    ValidateCRC16Test,
    ::testing::Values(
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xCB}, true),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x00}, false),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x04, 0x3F, 0x75, 0xF0, 0x7B, 0xE3, 0xDE}, true),
        std::make_pair(std::vector<uint8_t>{0x01, 0x03, 0x08, 0x3F, 0x75, 0xE3, 0xD2, 0x41,
                                                                        0xB6, 0x1C, 0x20, 0xA0, 0xC7}, true)
    )
);
