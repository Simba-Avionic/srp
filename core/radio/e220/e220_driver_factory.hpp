/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.hpp
 * @brief Factory of the `ebyte_e220` driver for MavlinkDaemon: driver file -> RadioDriver + E220Modem
 * on the srp UartDriver and GpioDriver (simulations in the sim build). A configuration error neither
 * throws nor returns nullptr: a driver is created whose Start() returns kPersistent with a readable
 * description (daemon log); the daemon retries Start() after its restart delay.
 *
 * The driver is registered statically in the daemon (`e220_driver.cc`,
 * SRP_MAVLINK_REGISTER_DRIVER("ebyte_e220", ...)); there is no plugin `.so`.
 */
#ifndef CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
#define CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "ara/com/mavlink/radio_driver.h"
#include "core/gpio/Igpio_driver.hpp"
#include "core/radio/e220/e220_modem.hpp"
#include "core/radio/e220/e220_registers.hpp"
#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

/// Driver id in the daemon configuration (`links.<name>.driver`).
constexpr const char* kE220DriverId = "ebyte_e220";

/// GPIO numbers given to the pins of the test hardware (`pins.ctl_dir`, no real GPIO): M0, M1, AUX.
constexpr E220Pins kCtlPins{1, 2, 3};

/// What the modem runs on.
struct E220Hardware {
  std::shared_ptr<uart::IUartDriver> uart;
  std::shared_ptr<gpio::IGpioDriver> gpio;
};

/// Provides the UART and GPIO for a parsed driver file; nullopt + a description in `error` = refuse it.
using E220HardwareProvider = std::function<std::optional<E220Hardware>(const E220DriverConfig& config,
                                                                      std::string* error)>;

/// The real hardware: srp UartDriver and GpioDriver (sysfs; simulations in the sim build).
/// `pins.ctl_dir` (the test pins) is refused.
std::optional<E220Hardware> ProductionHardware(const E220DriverConfig& config, std::string* error);

/// Builds the driver from the driver file (read once, when the link is created) on the given hardware.
std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220HardwareProvider& hardware,
                                                                 const E220Timing& timing = {});

/// The `ebyte_e220` driver on the real hardware.
std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path);

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_DRIVER_FACTORY_HPP_
