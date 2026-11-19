/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.hpp
 * @brief Factory of the `ebyte_e220` driver for MavlinkDaemon: configuration file -> E220RadioDriver
 * on the real UART (platform SerialPort) and sysfs GPIO pins. A configuration error neither throws
 * nor returns nullptr: a driver is created whose Start() returns kPersistent with a readable
 * description (daemon log); the daemon retries Start() after its restart delay.
 */
#ifndef CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
#define CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_

#include <memory>
#include <string>

#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_radio_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

/// Driver id in the daemon configuration (`links.<name>.driver`).
constexpr const char* kE220DriverId = "ebyte_e220";

struct E220FactoryOptions {
  /// Test build of the plugin only: `pins.ctl_dir` (pins in the files of the module simulator) is allowed.
  bool allow_ctl_dir{false};
  /// sysfs GPIO directory (tests pass a temporary directory).
  std::string sysfs_gpio_root{"/sys/class/gpio"};
  E220Timing timing;
};

/// Builds the driver from the configuration file (read once, when the link is created).
std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220FactoryOptions& options = {});

/// Registers `ebyte_e220` in the daemon registry (called from the plugin).
void RegisterE220Driver(ara::com::mavlink::IRadioDriverRegistry& registry,  // NOLINT(runtime/references)
                        E220FactoryOptions options = {});

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
