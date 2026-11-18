/**
 * Copyright (c) 2024
 *
 * @file e220_plugin.cc
 * @brief Plugin `.so` `ebyte_e220` dla MavlinkDaemona (reguła bazela mavlink_driver_plugin):
 * rejestruje driver o id `ebyte_e220`. Konfiguracja z pliku wskazanego przez `driver_config`.
 */
#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_driver_factory.hpp"

namespace {
void RegisterDrivers(ara::com::mavlink::IRadioDriverRegistry& registry) {  // NOLINT(runtime/references)
  srp::core::radio::e220::RegisterE220Driver(registry);
}
}  // namespace

SRP_MAVLINK_DRIVER_PLUGIN(RegisterDrivers)
