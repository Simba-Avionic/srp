/**
 * Copyright (c) 2024
 *
 * @file e220_test_plugin.cc
 * @brief TEST ONLY: the `ebyte_e220` plugin like the production one, but with `pins.ctl_dir` (the
 * M0/M1/AUX pins in the files of the e220_sim.py module simulator) - for host E2E tests.
 */
#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_driver_factory.hpp"

namespace {
void RegisterDrivers(ara::com::mavlink::IRadioDriverRegistry& registry) {  // NOLINT(runtime/references)
  srp::core::radio::e220::E220FactoryOptions opts;
  opts.allow_ctl_dir = true;
  srp::core::radio::e220::RegisterE220Driver(registry, opts);
}
}  // namespace

SRP_MAVLINK_DRIVER_PLUGIN(RegisterDrivers)
