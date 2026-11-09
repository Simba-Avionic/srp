/**
 * @file sim_i2c_test.cc
 * @brief UT of the sim I2C bus (core/i2c/sim_i2c_driver.cpp) through the I2CDriver interface: `i2c reset` restores only
 *        the injected state (ADC, EEPROM), `i2c reset-all` the whole model.
 *
 * @copyright Copyright (c) 2025
 */
#include <gtest/gtest.h>
#include <stdlib.h>
#include <unistd.h>
#include <filesystem>  // NOLINT
#include <fstream>
#include <string>
#include <vector>
#include "core/i2c/i2c_driver.hpp"

namespace {
constexpr uint8_t kPca = 0x70;
constexpr uint8_t kAds = 0x48;

class SimI2cTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char tmpl[] = "/tmp/srpsim_i2cXXXXXX";
    ASSERT_NE(mkdtemp(tmpl), nullptr);
    dir_ = tmpl;
    setenv("SRP_SIM_DIR", tmpl, 1);
    ASSERT_EQ(drv_.Init(), srp::core::ErrorCode::kOk);
  }

  void TearDown() override {
    std::error_code ec;
    std::filesystem::remove_all(dir_, ec);
  }

  // every ctl line gets a unique trailing comment, so it is a new command even when the text repeats
  void Ctl(const std::string& line) {
    std::ofstream f(dir_ + "/ctl", std::ios::app);
    f << line << " # " << ++n_ << "\n";
  }

  int PcaReg(uint8_t reg) {
    drv_.Ioctl(kPca);
    auto r = drv_.ReadWrite(reg, 1);
    return r ? (*r)[0] : -1;
  }

  int AdcCh0() {
    drv_.Ioctl(kAds);
    drv_.Write(static_cast<uint8_t>(0x00));  // select channel 0
    auto r = drv_.Read(2);
    return r ? (((*r)[0] << 8) | (*r)[1]) : -1;
  }

  srp::core::i2c::I2CDriver drv_;
  std::string dir_;
  int n_{0};
};

TEST_F(SimI2cTest, ResetKeepsRegistersWrittenByApplicationsAndClearsInjectedAdc) {
  Ctl("i2c adc 0 3.3");
  EXPECT_EQ(AdcCh0(), 4095);
  drv_.Ioctl(kPca);
  ASSERT_EQ(drv_.Write(std::vector<uint8_t>{0x08, 0x66}), srp::core::ErrorCode::kOk);
  ASSERT_EQ(PcaReg(0x08), 0x66);

  Ctl("i2c reset");
  EXPECT_EQ(PcaReg(0x08), 0x66) << "`i2c reset` must not clear registers written by the application";
  EXPECT_EQ(AdcCh0(), 0) << "`i2c reset` must clear the injected ADC voltages";
}

TEST_F(SimI2cTest, ResetRestoresDefaultEepromConfig) {
  Ctl("i2c eeprom-cfg nan a b c");
  drv_.Ioctl(0x50);
  // pointer {hi, lo} then read 4 bytes: the float xo is NaN now (0x7fc00000)
  auto r = drv_.ReadWrite(std::vector<uint8_t>{0x00, 0x00}, 4);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ((*r)[3], 0x7f);
  Ctl("i2c reset");
  drv_.Ioctl(0x50);
  r = drv_.ReadWrite(std::vector<uint8_t>{0x00, 0x00}, 4);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ((*r)[3], 0x3f) << "xo must be 1.0f (0x3f800000) after `i2c reset`";
}

TEST_F(SimI2cTest, ResetAllClearsEverything) {
  drv_.Ioctl(kPca);
  ASSERT_EQ(drv_.Write(std::vector<uint8_t>{0x08, 0x66}), srp::core::ErrorCode::kOk);
  ASSERT_EQ(PcaReg(0x08), 0x66);
  Ctl("i2c reset-all");
  EXPECT_EQ(PcaReg(0x08), 0);
  std::ifstream f(dir_ + "/state/i2c/0x70.hex");
  std::string hex;
  std::getline(f, hex);
  EXPECT_EQ(hex.substr(16, 2), "00") << "the state file of the PCA9685 must be rewritten by reset-all";
}
}  // namespace
