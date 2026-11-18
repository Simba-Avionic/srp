/**
 * Copyright (c) 2024
 *
 * @file test_e220_contract.cpp
 * @brief Testy kontraktu IRadioDriver (zestaw z srp_platform) uruchomione na E220RadioDriver z podstawka
 * modulu: bajty z radia, Send w calosci, Send nieblokujace, kBusy + OnTxReady, zero callbackow po Stop()
 * (takze Stop w trakcie czekania na OnTxReady po kBusy), Start po Stop, callbacki z watku drivera.
 */
#include <gtest/gtest.h>

#include <chrono>  // NOLINT
#include <memory>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/testing/radio_driver_contract_test.h"
#include "core/radio/e220/e220_radio_driver.hpp"
#include "core/radio/e220/testing/fake_e220_module.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {
namespace {
using ara::com::mavlink::testing::RadioContractParam;
using ara::com::mavlink::testing::RadioDriverContractTest;
using ara::com::mavlink::testing::RadioDriverHarness;
using testing::FakeE220Module;
using std::chrono::milliseconds;

RadioDriverHarness MakeHarness() {
  auto module = std::make_shared<FakeE220Module>();
  E220Timing t;
  t.aux_timeout = milliseconds(200);
  t.mode_switch_wait = milliseconds(2);
  t.mode_settle = milliseconds(0);
  t.command_timeout = milliseconds(200);
  t.retry_delay = milliseconds(5);
  t.aux_fall_wait = milliseconds(2);
  t.read_slice = milliseconds(20);
  E220Config cfg;
  cfg.channel = 20;
  cfg.air_rate_bps = 62500;  // szybki drenaz modelu bufora (testy kontraktu nie czekaja sekundami)
  RadioDriverHarness h;
  h.driver = std::make_shared<E220RadioDriver>("contract", "/dev/ttyTEST", cfg, module, module, t);
  h.peer_write = [module](const std::vector<std::uint8_t>& v) { module->InjectRx(v); };
  h.peer_read = [module](std::size_t want, std::uint32_t timeout_ms) {
    const auto end = std::chrono::steady_clock::now() + milliseconds(timeout_ms);
    std::vector<std::uint8_t> out;
    while (out.empty() && std::chrono::steady_clock::now() < end) {
      out = module->DrainTx(want);
      if (out.empty()) std::this_thread::sleep_for(milliseconds(2));
    }
    return out;
  };
  h.overflow_bytes = 400 * 20;  // ramki jedna po drugiej szybciej, niz model bufora (400 B) sie oprozni
  return h;
}

INSTANTIATE_TEST_SUITE_P(E220WithFakeModule, RadioDriverContractTest,
                         ::testing::Values(RadioContractParam{"e220_fake", MakeHarness}),
                         ara::com::mavlink::testing::RadioContractParamName);

}  // namespace
}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
