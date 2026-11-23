/**
 * Copyright (c) 2024
 *
 * @file test_radio_driver_contract.cpp
 * @brief The IRadioDriver contract suite (from srp_platform) run on RadioDriver + FakeModem, in the half-
 * and the full-duplex variant (the harness also emits link statistics through FakeModem::PushStats).
 */
#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "core/radio/common/radio_driver.hpp"
#include "core/radio/common/testing/fake_modem.hpp"

namespace {

using ara::com::mavlink::testing::RadioContractParam;
using ara::com::mavlink::testing::RadioDriverContractTest;
using ara::com::mavlink::testing::RadioDriverHarness;
using srp::core::radio::RadioDriver;
using srp::core::radio::testing::FakeModem;
using srp::core::radio::testing::FakeModemOptions;

RadioDriverHarness MakeHarness(bool full_duplex) {
  FakeModemOptions opt;
  opt.caps = {255, false, full_duplex};
  auto modem = std::make_shared<FakeModem>(opt);
  RadioDriverHarness h;
  h.driver = std::make_shared<RadioDriver>("contract", modem);
  h.peer_write = [modem](const std::vector<std::uint8_t>& b) { modem->PeerWrite(b); };
  h.peer_read = [modem](std::size_t want, std::uint32_t timeout_ms) { return modem->PeerRead(want, timeout_ms); };
  // the air buffer (400 B) + the frame in Transmit() + the frame in the slot, with a margin
  h.overflow_bytes = 1200;
  h.emit_link_stats = [modem](const ara::com::mavlink::RadioLinkStats& s) { modem->PushStats(s); };
  return h;
}

}  // namespace

INSTANTIATE_TEST_SUITE_P(RadioDriverFake, RadioDriverContractTest,
                         ::testing::Values(RadioContractParam{"half_duplex", [] { return MakeHarness(false); }},
                                           RadioContractParam{"full_duplex", [] { return MakeHarness(true); }}),
                         ara::com::mavlink::testing::RadioContractParamName);
