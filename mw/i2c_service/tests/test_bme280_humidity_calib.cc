/**
 * @file test_bme280_humidity_calib.cc
 * @brief dig_H4 / dig_H5 to signed 12 bit (F-42)
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "gtest/gtest.h"
#include "mw/i2c_service/controller/bme280/controller.hpp"

using srp::i2c::BME280;

TEST(Bme280HumidityCalibTest, PositiveValuesUnchanged) {
  EXPECT_EQ(BME280::ComposeDigH4(0x13, 0x25), 0x135);   // 309
  EXPECT_EQ(BME280::ComposeDigH5(0x35, 0x03), 0x033);   // 51 (E5[7:4]=3, E6=0x03)
  EXPECT_EQ(BME280::ComposeDigH4(0x00, 0x00), 0);
  EXPECT_EQ(BME280::ComposeDigH4(0x7F, 0x0F), 2047);
}

TEST(Bme280HumidityCalibTest, NegativeValuesAreSignExtended) {
  EXPECT_EQ(BME280::ComposeDigH4(0xF0, 0x00), -256);    // przed poprawka: 3840
  EXPECT_EQ(BME280::ComposeDigH4(0x80, 0x00), -2048);
  EXPECT_EQ(BME280::ComposeDigH4(0xFF, 0x0F), -1);
  EXPECT_EQ(BME280::ComposeDigH5(0x00, 0xF0), -256);
  EXPECT_EQ(BME280::ComposeDigH5(0xF0, 0xFF), -1);
  EXPECT_EQ(BME280::ComposeDigH5(0x00, 0x80), -2048);
}
