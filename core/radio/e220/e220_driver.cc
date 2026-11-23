/**
 * Copyright (c) 2024
 *
 * @file e220_driver.cc
 * @brief Static registration of the `ebyte_e220` driver in the daemon registry (library with
 * `alwayslink = True`, linked in by `mavlink_daemon_binary(drivers = [...])`).
 */
#include <memory>
#include <string>

#include "ara/com/mavlink/radio_driver_registry.h"
#include "core/radio/e220/e220_driver_factory.hpp"

SRP_MAVLINK_REGISTER_DRIVER(srp::core::radio::e220::kE220DriverId, [](const std::string& config_path) {
  return srp::core::radio::e220::CreateE220Driver(config_path);
});
