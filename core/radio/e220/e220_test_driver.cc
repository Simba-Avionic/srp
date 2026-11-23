/**
 * Copyright (c) 2024
 *
 * @file e220_test_driver.cc
 * @brief TEST ONLY registration of `ebyte_e220`: like e220_driver.cc, but `pins.ctl_dir` is allowed (the
 * pins are the files of the e220_sim.py simulator, the UART is the real UartDriver on its pty). Used by
 * the E2E test through mavlink_daemon_binary(drivers = [":e220_test_driver"]).
 */
#include <memory>
#include <string>

#include "ara/com/mavlink/radio_driver_registry.h"
#include "core/radio/e220/e220_driver_factory.hpp"
#include "core/radio/e220/testing/file_gpio_driver.hpp"
#include "core/uart/uart_driver.hpp"

namespace {
std::optional<srp::core::radio::e220::E220Hardware> TestHardware(const srp::core::radio::e220::E220DriverConfig& config,
                                                                 std::string* error) {
  using srp::core::radio::e220::E220Hardware;
  if (config.pins.ctl_dir.empty()) {
    return srp::core::radio::e220::ProductionHardware(config, error);
  }
  return E220Hardware{std::make_shared<srp::core::uart::UartDriver>(),
                      std::make_shared<srp::core::radio::e220::testing::FileGpioDriver>(config.pins.ctl_dir)};
}
}  // namespace

SRP_MAVLINK_REGISTER_DRIVER(srp::core::radio::e220::kE220DriverId, [](const std::string& config_path) {
  return srp::core::radio::e220::CreateE220Driver(config_path, TestHardware);
});
