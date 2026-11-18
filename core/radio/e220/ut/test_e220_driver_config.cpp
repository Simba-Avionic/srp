/**
 * Copyright (c) 2024
 *
 * @file test_e220_driver_config.cpp
 * @brief Plik konfiguracji drivera `ebyte_e220`: pola, wartosci domyslne, czytelne bledy; fabryka
 * (blad konfiguracji = driver, ktorego Start() zwraca kPersistent z opisem).
 */
#include <gtest/gtest.h>
#include <unistd.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "ara/com/mavlink/transport_error.h"
#include "core/radio/e220/e220_driver_config.hpp"
#include "core/radio/e220/e220_driver_factory.hpp"
#include "core/radio/e220/testing/recording_listener.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::IsTransportError;
using ara::com::mavlink::TransportErrc;

TEST(E220DriverConfig, ParsesFullConfiguration) {
  const auto r = ParseDriverConfigJson(R"({
    "uart": "/dev/ttyS2",
    "pins": {"m0": 66, "m1": 67, "aux": 69},
    "e220": {"address": 4660, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600,
             "parity": "8E1", "packet_length": 128, "lbt": true, "wor_cycle": 3}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.uart, "/dev/ttyS2");
  EXPECT_EQ(r.config.pins.m0, 66);
  EXPECT_EQ(r.config.pins.m1, 67);
  EXPECT_EQ(r.config.pins.aux, 69);
  EXPECT_EQ(r.config.radio.address, 4660);
  EXPECT_EQ(r.config.radio.channel, 18);
  EXPECT_EQ(r.config.radio.air_rate_bps, 9600U);
  EXPECT_EQ(r.config.radio.power_dbm, 27);
  EXPECT_EQ(r.config.radio.uart_baudrate, 57600U);
  EXPECT_EQ(r.config.radio.parity, UartParity::k8E1);
  EXPECT_EQ(r.config.radio.packet_length, 128);
  EXPECT_TRUE(r.config.radio.lbt);
  EXPECT_EQ(r.config.radio.wor_cycle.value_or(99), 3);
}

TEST(E220DriverConfig, RadioSectionIsOptionalWithDefaults) {
  const auto r = ParseDriverConfigJson(R"({"uart": "/dev/ttyS2", "pins": {"m0": 1, "m1": 2, "aux": 3}})");
  ASSERT_TRUE(r.ok) << r.error;
  EXPECT_EQ(r.config.radio, E220Config{});
}

struct BadCase {
  const char* json;
  const char* needle;
};

TEST(E220DriverConfig, ErrorsNameTheFieldAndWhatIsWrong) {
  const BadCase cases[] = {
      {"{", "niepoprawny JSON"},
      {"[]", "obiektem"},
      {R"({"pins": {"m0":1,"m1":2,"aux":3}})", "'uart'"},
      {R"({"uart": "", "pins": {"m0":1,"m1":2,"aux":3}})", "'uart'"},
      {R"({"uart": "/dev/x"})", "'pins'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2}})", "pins: brak pola 'aux'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":"69"}})", "pins.aux"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":1,"aux":3}})", "roznymi"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3,"cs":4}})", "pins: nieznane pole 'cs'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "baud": 1})", "nieznane pole 'baud'"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"channel": 99}})", "e220:"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"chanel": 5}})", "chanel"},
      {R"({"uart": "/dev/x", "pins": {"m0":1,"m1":2,"aux":3}, "e220": {"air_rate": 1234}})", "e220:"},
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

TEST(E220DriverFactory, BadConfigGivesDriverWhoseStartFailsPersistentlyWithTheReason) {
  testing::RecordingListener lis;
  const std::string path = "/tmp/srp_e220_badcfg_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << R"({"uart": "/dev/ttyS9", "pins": {"m0": 1, "m1": 2, "aux": 3}, "e220": {"channel": 200}})";
  const auto d = CreateE220Driver(path);
  ASSERT_NE(d, nullptr);
  const auto r = d->Start(lis, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  const auto text = r.Error().SupportData();
  EXPECT_NE(text.find(path), std::string::npos) << text;
  EXPECT_NE(text.find("e220:"), std::string::npos) << text;
  d->Stop();
  std::remove(path.c_str());
}

TEST(E220DriverFactory, ConfigFileIsReloadedOnEveryStart) {
  testing::RecordingListener lis;
  const std::string path = "/tmp/srp_e220_reload_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << R"({"uart": "/dev/ttyS9", "pins": {"m0": 1, "m1": 2, "aux": 3}, "bogus": 1})";
  E220FactoryOptions opts;
  opts.sysfs_gpio_root = "/tmp/srp_e220_no_gpio_root";  // nie dotykamy prawdziwego /sys/class/gpio
  opts.timing.attempts = 1;
  const auto d = CreateE220Driver(path, opts);
  const auto bad = d->Start(lis, {});
  ASSERT_FALSE(bad.HasValue());
  EXPECT_TRUE(IsTransportError(bad.Error(), TransportErrc::kPersistent));
  EXPECT_NE(bad.Error().SupportData().find("nieznane pole 'bogus'"), std::string::npos);
  d->Stop();
  std::ofstream(path, std::ios::trunc) << R"({"uart": "/dev/nie_ma_uartu", "pins": {"m0": 1, "m1": 2, "aux": 3}})";
  const auto fixed = d->Start(lis, {});  // config poprawny: blad dopiero na sprzecie (brak UART/GPIO), nie configu
  ASSERT_FALSE(fixed.HasValue());
  EXPECT_TRUE(IsTransportError(fixed.Error(), TransportErrc::kRetryable)) << fixed.Error().SupportData();
  d->Stop();
  std::remove(path.c_str());
}

TEST(E220DriverFactory, MissingConfigPathAndMissingFileAreStartErrors) {
  testing::RecordingListener lis;
  for (const std::string path : {std::string{}, std::string{"/tmp/srp_e220_missing_cfg.json"}}) {
    const auto d = CreateE220Driver(path);
    ASSERT_NE(d, nullptr);
    const auto r = d->Start(lis, {});
    ASSERT_FALSE(r.HasValue());
    EXPECT_TRUE(IsTransportError(r.Error(), TransportErrc::kPersistent));
  }
}

TEST(E220DriverFactory, CtlDirPinsAreRejectedInTheProductionPlugin) {
  testing::RecordingListener lis;
  const std::string path = "/tmp/srp_e220_ctl_" + std::to_string(::getpid()) + ".json";
  std::ofstream(path) << R"({"uart": "/dev/ttyS9", "pins": {"ctl_dir": "/tmp/sim"}})";
  const auto d = CreateE220Driver(path);  // allow_ctl_dir = false
  const auto r = d->Start(lis, {});
  ASSERT_FALSE(r.HasValue());
  EXPECT_NE(r.Error().SupportData().find("pluginie testowym"), std::string::npos) << r.Error().SupportData();
  std::remove(path.c_str());
}

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
