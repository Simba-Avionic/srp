/**
 * @file test_crc_16.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <utility>
#include <vector>

#include "core/common/crc_16.h"
class CRC16TestFixture : public testing::Test,
                         public ::testing::WithParamInterface<
                             std::pair<uint16_t, std::vector<uint8_t>>> {};

TEST_P(CRC16TestFixture, Vector_test) {
  const auto p = GetParam();
  ASSERT_EQ(p.first, simba::core::CRC16::calculate(p.second));
}

INSTANTIATE_TEST_SUITE_P(
    CRC16, CRC16TestFixture,
    testing::Values(
        std::pair<uint16_t, std::vector<uint8_t>>{0x84C0,
                                                  {0x00, 0x00, 0x00, 0x00}},
        std::pair<uint16_t, std::vector<uint8_t>>{0xD374, {0xF2, 0x01, 0x83}},
        std::pair<uint16_t, std::vector<uint8_t>>{0x2023,
                                                  {0x0F, 0xAA, 0x00, 0x55}},
        std::pair<uint16_t, std::vector<uint8_t>>{0xB8F9,
                                                  {0x00, 0xFF, 0x55, 0x11}},
        std::pair<uint16_t, std::vector<uint8_t>>{
            0xF53F, {0x33, 0x22, 0x55, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}},
        std::pair<uint16_t, std::vector<uint8_t>>{0x0745, {0x92, 0x6B, 0x55}},
        std::pair<uint16_t, std::vector<uint8_t>>{0x1D0F,
                                                  {0xFF, 0xFF, 0xFF, 0xFF}}));

TEST(CRC16, value_uint8) {
  ASSERT_EQ(0x1ECE, simba::core::CRC16::calculate((uint8_t)0xF1));
}
TEST(CRC16, value_uint16) {
  ASSERT_EQ(0xE231, simba::core::CRC16::calculate((uint16_t)0xF1));
}
TEST(CRC16, value_uint32) {
  ASSERT_EQ(0x7BFE, simba::core::CRC16::calculate((uint32_t)0xF1));
}
TEST(CRC16, value_uint64) {
  ASSERT_EQ(0xCE00, simba::core::CRC16::calculate((uint64_t)0xF1));
}
TEST(CRC16, value_int8) {
  ASSERT_EQ(0x1ECE, simba::core::CRC16::calculate((int8_t)0xF1));
}
TEST(CRC16, value_int16) {
  ASSERT_EQ(0xE231, simba::core::CRC16::calculate((int16_t)0xF1));
}
TEST(CRC16, value_int32) {
  ASSERT_EQ(0x7BFE, simba::core::CRC16::calculate((int32_t)0xF1));
}
TEST(CRC16, value_int64) {
  ASSERT_EQ(0xCE00, simba::core::CRC16::calculate((int64_t)0xF1));
}
