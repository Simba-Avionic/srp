/**
 * Copyright (c) 2024
 *
 * @file e220_test_plugin.cc
 * @brief TESTONLY: plugin `ebyte_e220` jak produkcyjny, ale z `pins.ctl_dir` (piny M0/M1/AUX w
 * plikach atrapy modemu e220_sim.py) i krotkimi czasami — do E2E z atrapa na hoscie.
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
