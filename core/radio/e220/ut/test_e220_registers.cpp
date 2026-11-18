/**
 * Copyright (c) 2024
 *
 * @file test_e220_registers.cpp
 * @brief E220 registers (Ebyte manual, sections 6.1-6.4), the JSON radio configuration and the
 * driver configuration file.
 */
#include <gtest/gtest.h>

#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "core/radio/e220/e220_registers.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {

const Registers kFactory{0x00, 0x00, 0x62, 0x00, 0x17, 0x03};  // 6.4 (p. 35)

TEST(E220Registers, FactoryDefaultsNeedNoChangesForDefaultConfig) {
  const E220Config cfg;  // the default configuration = the factory settings
  const auto desired = DesiredRegisters(cfg, kFactory);
  EXPECT_EQ(desired, kFactory);
  EXPECT_TRUE(DifferingRegisters(kFactory, desired, cfg).empty());
}

TEST(E220Registers, ManualExampleAddress1234Baud9600AirRate24k) {
  // 6.1 example 2: C0 00 03 12 34 62
  E220Config cfg;
  cfg.address = 0x1234;
  const auto d = DesiredRegisters(cfg, kFactory);
  EXPECT_EQ(d[kRegAddrH], 0x12);
  EXPECT_EQ(d[kRegAddrL], 0x34);
  EXPECT_EQ(d[kReg0], 0x62);
}

TEST(E220Registers, ChannelIsWrittenToReg2) {
  E220Config cfg;
  cfg.channel = 9;  // 6.1 example 1: C0 04 01 09
  EXPECT_EQ(DesiredRegisters(cfg, kFactory)[kReg2], 0x09);
}

TEST(E220Registers, BaudCodesFollowTable) {
  const std::vector<std::pair<std::uint32_t, std::uint8_t>> cases = {
      {1200, 0}, {2400, 1}, {4800, 2}, {9600, 3}, {19200, 4}, {38400, 5}, {57600, 6}, {115200, 7}};
  for (const auto& [baud, code] : cases) {
    ASSERT_TRUE(BaudCode(baud).has_value());
    EXPECT_EQ(*BaudCode(baud), code) << baud;
    E220Config cfg;
    cfg.uart_baudrate = baud;
    EXPECT_EQ(DesiredRegisters(cfg, kFactory)[kReg0] >> 5, code);
  }
  EXPECT_FALSE(BaudCode(14400).has_value());
}

TEST(E220Registers, AirRateCodesAndEquivalence) {
  EXPECT_EQ(*AirRateCode(2400), 2);  // 010 = default (000/001 are 2.4k too)
  EXPECT_EQ(*AirRateCode(4800), 3);
  EXPECT_EQ(*AirRateCode(9600), 4);
  EXPECT_EQ(*AirRateCode(19200), 5);
  EXPECT_EQ(*AirRateCode(38400), 6);
  EXPECT_EQ(*AirRateCode(62500), 7);
  EXPECT_FALSE(AirRateCode(1200).has_value());
  const E220Config cfg;
  EXPECT_TRUE(RegisterMatches(kReg0, 0x60, 0x62, cfg));   // 000 == 010 (2.4k)
  EXPECT_TRUE(RegisterMatches(kReg0, 0x61, 0x62, cfg));   // 001 == 010
  EXPECT_FALSE(RegisterMatches(kReg0, 0x63, 0x62, cfg));  // 4.8k != 2.4k
}

TEST(E220Registers, ParityBitsAndEquivalence) {
  E220Config cfg;
  cfg.parity = UartParity::k8O1;
  EXPECT_EQ((DesiredRegisters(cfg, kFactory)[kReg0] >> 3) & 3, 1);
  cfg.parity = UartParity::k8E1;
  EXPECT_EQ((DesiredRegisters(cfg, kFactory)[kReg0] >> 3) & 3, 2);
  const E220Config none;
  EXPECT_TRUE(RegisterMatches(kReg0, 0x62 | (3 << 3), 0x62, none));  // 11 == 8N1
}

TEST(E220Registers, PowerCodesForT30S) {
  EXPECT_EQ(*PowerCode(30), 0);
  EXPECT_EQ(*PowerCode(27), 1);
  EXPECT_EQ(*PowerCode(24), 2);
  EXPECT_EQ(*PowerCode(21), 3);
  EXPECT_FALSE(PowerCode(22).has_value());  // 22 dBm is the T22 variant, not the T30S
  E220Config cfg;
  cfg.power_dbm = 21;
  EXPECT_EQ(DesiredRegisters(cfg, kFactory)[kReg1] & 3, 3);
}

TEST(E220Registers, PacketLengthCodes) {
  EXPECT_EQ(*PacketCode(200), 0);
  EXPECT_EQ(*PacketCode(128), 1);
  EXPECT_EQ(*PacketCode(64), 2);
  EXPECT_EQ(*PacketCode(32), 3);
  E220Config cfg;
  cfg.packet_length = 32;
  EXPECT_EQ(DesiredRegisters(cfg, kFactory)[kReg1] >> 6, 3);
}

TEST(E220Registers, Reg3IsForcedTransparentWithoutRssiByte) {
  Registers cur = kFactory;
  cur[kReg3] = 0xC3;  // RSSI byte + fixed + WOR 3
  const E220Config cfg;
  const auto d = DesiredRegisters(cfg, cur);
  EXPECT_EQ(d[kReg3] & 0x80, 0);  // no RSSI byte
  EXPECT_EQ(d[kReg3] & 0x40, 0);  // transparent transmission (fixed = 0)
  EXPECT_EQ(d[kReg3] & 0x07, 0x03);  // WOR untouched
  EXPECT_EQ(DifferingRegisters(cur, d, cfg), std::vector<std::size_t>{kReg3});
}

TEST(E220Registers, Reg1AmbientRssiIsClearedAndUnverifiedBitsStayAsRead) {
  Registers cur = kFactory;
  cur[kReg1] = 0x3C;  // bit5 (ambient RSSI), bits 4:3 reserved, bit2 (unverified, see README)
  const E220Config cfg;
  const auto d = DesiredRegisters(cfg, cur);
  EXPECT_EQ(d[kReg1] & 0x20, 0);
  EXPECT_EQ(d[kReg1] & 0x1C, 0x1C);  // reserved bits and bit 2 are left as read
  EXPECT_EQ(DifferingRegisters(cur, d, cfg).size(), 1U);
  Registers only_bit2 = kFactory;
  only_bit2[kReg1] = 0x04;
  EXPECT_TRUE(DifferingRegisters(only_bit2, DesiredRegisters(cfg, only_bit2), cfg).empty());  // never written
}

TEST(E220Registers, LbtBitFollowsConfig) {
  E220Config cfg;
  cfg.lbt = true;
  EXPECT_EQ(DesiredRegisters(cfg, kFactory)[kReg3] & 0x10, 0x10);
  Registers cur = kFactory;
  cur[kReg3] |= 0x10;
  cfg.lbt = false;
  EXPECT_EQ(DesiredRegisters(cfg, cur)[kReg3] & 0x10, 0);
}

TEST(E220Registers, WorCycleManagedOnlyWhenConfigured) {
  Registers cur = kFactory;
  cur[kReg3] = 0x05;
  E220Config cfg;
  EXPECT_TRUE(RegisterMatches(kReg3, 0x05, 0x03, cfg));  // WOR is not managed
  cfg.wor_cycle = 3;
  EXPECT_EQ(DesiredRegisters(cfg, cur)[kReg3] & 7, 3);
  EXPECT_FALSE(RegisterMatches(kReg3, 0x05, DesiredRegisters(cfg, cur)[kReg3], cfg));
}

TEST(E220Registers, OneChangedParameterGivesOneDifferingRegister) {
  E220Config cfg;
  cfg.channel = 30;
  const auto d = DesiredRegisters(cfg, kFactory);
  EXPECT_EQ(DifferingRegisters(kFactory, d, cfg), std::vector<std::size_t>{kReg2});
  E220Config cfg2;
  cfg2.power_dbm = 24;
  EXPECT_EQ(DifferingRegisters(kFactory, DesiredRegisters(cfg2, kFactory), cfg2),
            std::vector<std::size_t>{kReg1});
  E220Config cfg3;
  cfg3.address = 0x0102;
  EXPECT_EQ(DifferingRegisters(kFactory, DesiredRegisters(cfg3, kFactory), cfg3),
            (std::vector<std::size_t>{kRegAddrH, kRegAddrL}));
}

TEST(E220Registers, AfterApplyingDesiredThereIsNothingToChange) {
  E220Config cfg;
  cfg.address = 0xABCD;
  cfg.channel = 40;
  cfg.air_rate_bps = 19200;
  cfg.power_dbm = 27;
  cfg.uart_baudrate = 115200;
  cfg.parity = UartParity::k8E1;
  cfg.packet_length = 64;
  cfg.lbt = true;
  const auto d = DesiredRegisters(cfg, kFactory);
  EXPECT_TRUE(DifferingRegisters(d, DesiredRegisters(cfg, d), cfg).empty());
}

TEST(E220Validate, AcceptsDefaultsAndRejectsOutOfRange) {
  EXPECT_EQ(Validate(E220Config{}), "");
  E220Config c;
  c.channel = 84;
  EXPECT_NE(Validate(c).find("channel"), std::string::npos);
  c = {};
  c.air_rate_bps = 1200;
  EXPECT_NE(Validate(c).find("air_rate"), std::string::npos);
  c = {};
  c.power_dbm = 22;
  EXPECT_NE(Validate(c).find("power_dbm"), std::string::npos);
  c = {};
  c.uart_baudrate = 230400;
  EXPECT_NE(Validate(c).find("uart_baudrate"), std::string::npos);
  c = {};
  c.packet_length = 100;
  EXPECT_NE(Validate(c).find("packet_length"), std::string::npos);
  c = {};
  c.wor_cycle = 9;
  EXPECT_NE(Validate(c).find("wor_cycle"), std::string::npos);
}

TEST(E220Json, ParsesFullConfig) {
  const auto r = ParseConfigJson(R"({"address": 4660, "channel": 18, "air_rate": 4800,
      "power_dbm": 27, "uart_baudrate": 57600, "parity": "8E1", "packet_length": 128,
      "lbt": true, "wor_cycle": 2})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.address, 4660);
  EXPECT_EQ(r.config.channel, 18);
  EXPECT_EQ(r.config.air_rate_bps, 4800U);
  EXPECT_EQ(r.config.power_dbm, 27);
  EXPECT_EQ(r.config.uart_baudrate, 57600U);
  EXPECT_EQ(r.config.parity, UartParity::k8E1);
  EXPECT_EQ(r.config.packet_length, 128);
  EXPECT_TRUE(r.config.lbt);
  EXPECT_EQ(*r.config.wor_cycle, 2);
}

TEST(E220Json, EmptyObjectGivesFactoryLikeDefaults) {
  const auto r = ParseConfigJson("{}");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config, E220Config{});
}

TEST(E220Json, AcceptsNestedE220Key) {
  const auto r = ParseConfigJson(R"({"uart": "/dev/ttyS1", "e220": {"channel": 5}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.channel, 5);
}

TEST(E220Json, RejectsUnknownKeyAndBadTypes) {
  for (const char* bad : {R"({"chanel": 5})", R"({"channel": "5"})", R"({"channel": 5.5})",
                          R"({"channel": -1})", R"({"lbt": 1})", R"({"parity": "8N2"})",
                          R"({"parity": 3})", R"([1,2])", "not json", R"({"address": 70000})"}) {
    const auto r = ParseConfigJson(bad);
    EXPECT_FALSE(r.ok) << bad;
    EXPECT_FALSE(r.error.empty()) << bad;
  }
}

TEST(E220Json, RejectsValuesOutsideChipRanges) {
  for (const char* bad : {R"({"channel": 84})", R"({"power_dbm": 22})", R"({"air_rate": 1200})",
                          R"({"uart_baudrate": 230400})", R"({"packet_length": 100})",
                          R"({"wor_cycle": 8})"}) {
    EXPECT_FALSE(ParseConfigJson(bad).ok) << bad;
  }
}

TEST(E220Describe, MentionsKeyParameters) {
  const auto s = DescribeRegisters(Registers{0x12, 0x34, 0xC2, 0x01, 0x12, 0x00});
  EXPECT_NE(s.find("0x1234"), std::string::npos);
  EXPECT_NE(s.find("57600"), std::string::npos);
  EXPECT_NE(s.find("ch=18"), std::string::npos);
  EXPECT_NE(s.find("transparent"), std::string::npos);
}


// ---------------------------------------------------------------- driver configuration file

TEST(E220DriverConfig, ParsesFullConfiguration) {
  const auto r = ParseDriverConfigJson(R"({
    "uart": "/dev/ttyS2",
    "pins": {"m0": 66, "m1": 67, "aux": 69},
    "aux_idle_level": 0,
    "e220": {"address": 4660, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600,
             "packet_length": 128, "lbt": true, "wor_cycle": 3}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.uart, "/dev/ttyS2");
  EXPECT_EQ(r.config.pins.m0, 66);
  EXPECT_EQ(r.config.pins.m1, 67);
  EXPECT_EQ(r.config.pins.aux, 69);
  EXPECT_FALSE(r.config.radio.aux_idle_level);
  EXPECT_EQ(r.config.radio.address, 4660);
  EXPECT_EQ(r.config.radio.channel, 18);
  EXPECT_EQ(r.config.radio.air_rate_bps, 9600U);
  EXPECT_EQ(r.config.radio.power_dbm, 27);
  EXPECT_EQ(r.config.radio.uart_baudrate, 57600U);
  EXPECT_EQ(r.config.radio.packet_length, 128);
  EXPECT_TRUE(r.config.radio.lbt);
  EXPECT_EQ(r.config.radio.wor_cycle.value_or(99), 3);
}

TEST(E220DriverConfig, RadioSectionAndAuxLevelAreOptionalWithDefaults) {
  const auto r = ParseDriverConfigJson(R"({"uart": "/dev/ttyS2", "pins": {"m0": 1, "m1": 2, "aux": 3}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.radio, E220Config{});
  EXPECT_TRUE(r.config.radio.aux_idle_level);
}

TEST(E220DriverConfig, ErrorsNameTheFieldAndWhatIsWrong) {
  struct BadCase {
    const char* json;
    const char* needle;
  };
  const BadCase cases[] = {
      {"{", "invalid JSON"},
      {"[]", "JSON object"},
      {R"({"pins": {"m0":1,"m1":2,"aux":3}})", "'uart'"},
      {R"({"uart": "", "pins": {"m0":1,"m1":2,"aux":3}})", "'uart'"},
      {R"({"uart": "/dev/x"})", "'pins'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2}})", "pins: missing field 'aux'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":"69"}})", "pins.aux"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":1,"aux":3}})", "different"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3,"cs":4}})", "pins: unknown field 'cs'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "baud": 1})", "unknown field 'baud'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "aux_idle_level": 2})", "aux_idle_level"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "aux_idle_level": "1"})", "aux_idle_level"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"channel": 99}})", "e220:"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"chanel": 5}})", "chanel"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"parity": "8E1"}})", "e220.parity"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": 5})", "'e220'"},
  };
  for (const auto& c : cases) {
    const auto r = ParseDriverConfigJson(c.json);
    EXPECT_FALSE(r.ok) << c.json;
    EXPECT_NE(r.error.find(c.needle), std::string::npos) << c.json << " -> " << r.error;
  }
}

TEST(E220DriverConfig, CtlDirPinsAreParsedForTheTestPlugin) {
  const auto r = ParseDriverConfigJson(R"({"uart": "/dev/x", "pins": {"ctl_dir": "/tmp/sim"}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.pins.ctl_dir, "/tmp/sim");
}

TEST(E220DriverConfig, LoadFromFileNamesTheFileInErrors) {
  const auto missing = LoadDriverConfig("/tmp/srp_e220_no_such_file.json");
  EXPECT_FALSE(missing.ok);
  EXPECT_NE(missing.error.find("/tmp/srp_e220_no_such_file.json"), std::string::npos);
  const std::string path = "/tmp/srp_e220_cfg_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << R"({"uart": "/dev/x"})";
  const auto bad = LoadDriverConfig(path);
  EXPECT_FALSE(bad.ok);
  EXPECT_NE(bad.error.find(path), std::string::npos);
  EXPECT_NE(bad.error.find("'pins'"), std::string::npos);
  std::remove(path.c_str());
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
