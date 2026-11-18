/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.cpp
 */
#include "core/radio/e220/e220_driver_factory.hpp"

#include <tuple>
#include <utility>

#include "core/radio/e220/serial_port_uart.hpp"
#include "core/radio/e220/sysfs_e220_pins.hpp"
#include "core/radio/e220/testing/file_e220_pins.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220FactoryOptions& options) {
  const std::string name = kE220DriverId;
  if (config_path.empty()) {
    return E220RadioDriver::WithConfigError(
        name, "no driver configuration file (set 'driver_config' of the link in the daemon configuration)");
  }
  const auto parsed = LoadDriverConfig(config_path);
  if (!parsed.ok) {
    return E220RadioDriver::WithConfigError(name, "driver configuration: " + parsed.error);
  }
  const auto& cfg = parsed.config;
  std::shared_ptr<IE220Pins> pins;
  if (!cfg.pins.ctl_dir.empty()) {
    if (!options.allow_ctl_dir) {
      return E220RadioDriver::WithConfigError(
          name, "file '" + config_path + "': pins.ctl_dir is available only in the test plugin; give GPIO "
                "numbers (pins.m0, pins.m1, pins.aux)");
    }
    pins = std::make_shared<testing::FileE220Pins>(cfg.pins.ctl_dir);
  } else {
    pins = std::make_shared<SysfsE220Pins>(cfg.pins.m0, cfg.pins.m1, cfg.pins.aux, options.sysfs_gpio_root);
  }
  return std::make_shared<E220RadioDriver>(name + "(" + cfg.uart + ")", cfg.uart, cfg.radio,
                                           std::make_shared<SerialPortUart>(), std::move(pins), options.timing);
}

void RegisterE220Driver(ara::com::mavlink::IRadioDriverRegistry& registry,  // NOLINT(runtime/references)
                        E220FactoryOptions options) {
  std::ignore = registry.Register(kE220DriverId, [options](const std::string& config_path) {
    return CreateE220Driver(config_path, options);
  });
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
