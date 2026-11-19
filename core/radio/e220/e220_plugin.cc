/**
 * Copyright (c) 2024
 *
 * @file e220_plugin.cc
 * @brief `ebyte_e220` plugin `.so` for MavlinkDaemon (bazel rule mavlink_driver_plugin):
 * registers the driver with id `ebyte_e220`. Configuration comes from the file given by `driver_config`.
 */
#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_driver_factory.hpp"

namespace {
void RegisterDrivers(ara::com::mavlink::IRadioDriverRegistry& registry) {  // NOLINT(runtime/references)
  srp::core::radio::e220::RegisterE220Driver(registry);
}
}  // namespace

SRP_MAVLINK_DRIVER_PLUGIN(RegisterDrivers)
