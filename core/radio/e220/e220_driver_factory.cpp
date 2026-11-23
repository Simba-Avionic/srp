/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.cpp
 */
#include "core/radio/e220/e220_driver_factory.hpp"

#include <chrono>  // NOLINT
#include <utility>

#include "core/file/file.hpp"
#include "core/gpio/gpio_driver.hpp"
#include "core/radio/common/radio_driver.hpp"
#include "core/uart/uart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
std::shared_ptr<ara::com::mavlink::IRadioDriver> ConfigErrorDriver(const std::string& error) {
  return std::make_shared<RadioDriver>(std::string("e220:") + kE220DriverId, E220Modem::WithConfigError(error));
}
}  // namespace

std::optional<E220Hardware> ProductionHardware(const E220DriverConfig& config, std::string* error) {
  if (!config.pins.ctl_dir.empty()) {
    *error = "pins.ctl_dir is available only in the test driver; give GPIO numbers (pins.m0, pins.m1, pins.aux)";
    return std::nullopt;
  }
  return E220Hardware{std::make_shared<uart::UartDriver>(),
                      std::make_shared<gpio::GpioDriver>(std::make_unique<FileHandler>())};
}

std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220HardwareProvider& hardware,
                                                                 const E220Timing& timing) {
  if (config_path.empty()) {
    return ConfigErrorDriver(
        "no driver configuration file (set 'driver_config' of the link in the daemon configuration)");
  }
  const auto parsed = LoadDriverConfig(config_path);
  if (!parsed.ok) {
    return ConfigErrorDriver("driver configuration: " + parsed.error);
  }
  const auto& cfg = parsed.config;
  std::string error;
  auto hw = hardware(cfg, &error);
  if (!hw) {
    return ConfigErrorDriver("file '" + config_path + "': " + error);
  }
  // The test pins (ctl_dir) have no GPIO numbers: they are numbered kCtlPins so that the three differ.
  const E220Pins pins = cfg.pins.ctl_dir.empty() ? E220Pins{cfg.pins.m0, cfg.pins.m1, cfg.pins.aux} : kCtlPins;
  auto modem = std::make_shared<E220Modem>(cfg.uart, cfg.radio, pins, std::move(hw->uart), std::move(hw->gpio), timing,
                                           std::chrono::milliseconds{cfg.stats_period_ms});
  return std::make_shared<RadioDriver>(std::string("e220:") + cfg.uart, std::move(modem));
}

std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path) {
  return CreateE220Driver(config_path, ProductionHardware);
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
