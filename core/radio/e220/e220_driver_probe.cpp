/**
 * Copyright (c) 2024
 *
 * @file e220_driver_probe.cpp
 * @brief TESTONLY: narzedzie E2E drivera E220 z atrapa modemu (e220_sim.py): prawdziwy E220RadioDriver na
 * prawdziwym UART (termios, pty atrapy) i pinach w plikach atrapy, bez daemona — surowy strumien bajtow
 * przez Send()/OnReceive().
 *
 * Tryby (--mode):
 *   start  konfiguruje modem i wypisuje JSON z rejestrami (idempotencja: 2. start bez zapisow C0),
 *   burst  --count N ramek po --size B (albo --min-size/--max-size, albo --sizes "a,b,c"), kazda osobnym
 *          Send() jedna po drugiej (kBusy -> czekanie na OnTxReady), na koncu czeka na oproznienie kolejki;
 *          wypisuje liczbe bajtow i FNV-1a 64 strumienia,
 *   sink   odbiera do --seconds i wypisuje liczbe bajtow + FNV-1a 64.
 * Piny M0/M1/AUX: pliki w katalogu ctl atrapy (FileE220Pins). Opcje: --config JSON (parametry radia,
 * jak `e220` w pliku drivera), --ignore-aux-write 1 (kontrola negatywna: bez pacingu i AUX).
 */
#include <chrono>  // NOLINT
#include <cinttypes>
#include <condition_variable>  // NOLINT
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <mutex>  // NOLINT
#include <sstream>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/transport_error.h"
#include "core/radio/e220/e220_radio_driver.hpp"
#include "core/radio/e220/e220_registers.hpp"
#include "core/radio/e220/termios_uart.hpp"
#include "core/radio/e220/testing/file_e220_pins.hpp"

namespace {
namespace e220 = srp::core::radio::e220;
using Clock = std::chrono::steady_clock;

std::string Arg(int argc, char** argv, const std::string& key, const std::string& def = "") {
  for (int i = 1; i + 1 < argc; ++i) {
    if (key == argv[i]) {
      return argv[i + 1];
    }
  }
  return def;
}

class Listener final : public ara::com::mavlink::IRadioListener {
 public:
  void OnReceive(std::span<const std::uint8_t> bytes) noexcept override {
    for (const auto b : bytes) {
      fnv_ = (fnv_ ^ b) * 1099511628211ULL;
    }
    total_ += bytes.size();
  }
  void OnTxReady() noexcept override {
    std::lock_guard<std::mutex> lock{mutex_};
    ++tx_ready_;
    cv_.notify_all();
  }
  void OnError(ara::core::ErrorCode e, bool fatal) noexcept override {
    std::fprintf(stderr, "OnError(fatal=%d): %s\n", fatal ? 1 : 0, e.SupportData().c_str());
    std::lock_guard<std::mutex> lock{mutex_};
    failed_ = failed_ || fatal;
    cv_.notify_all();
  }
  /// Czeka na kolejny OnTxReady (albo blad fatalny); false = timeout.
  bool WaitTxReady(int seen, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock{mutex_};
    return cv_.wait_for(lock, timeout, [&] { return tx_ready_ > seen || failed_; }) && !failed_;
  }
  int tx_ready() {
    std::lock_guard<std::mutex> lock{mutex_};
    return tx_ready_;
  }
  std::uint64_t total() const { return total_; }
  std::uint64_t fnv() const { return fnv_; }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  int tx_ready_{0};
  bool failed_{false};
  std::uint64_t total_{0};
  std::uint64_t fnv_{1469598103934665603ULL};
};
}  // namespace

int main(int argc, char** argv) {
  const std::string uart_path = Arg(argc, argv, "--uart");
  const std::string ctl_dir = Arg(argc, argv, "--ctl-dir");
  const std::string config_file = Arg(argc, argv, "--config");
  const std::string mode = Arg(argc, argv, "--mode", "start");
  const int seconds = std::atoi(Arg(argc, argv, "--seconds", "3").c_str());
  if (uart_path.empty() || ctl_dir.empty()) {
    std::fprintf(stderr,
                 "uzycie: e220_driver_probe --uart PTY --ctl-dir DIR [--config JSON] --mode start|burst|sink\n");
    return 2;
  }
  std::string json = "{}";
  if (!config_file.empty()) {
    std::ifstream in{config_file};
    std::stringstream ss;
    ss << in.rdbuf();
    json = ss.str();
  }
  const auto parsed = e220::ParseConfigJson(json);
  if (!parsed.ok) {
    std::fprintf(stderr, "konfiguracja E220: %s\n", parsed.error.c_str());
    return 2;
  }
  e220::E220Timing timing;
  timing.test_ignore_aux_on_write = Arg(argc, argv, "--ignore-aux-write", "0") == "1";  // kontrola negatywna
  auto driver = std::make_shared<e220::E220RadioDriver>(
      "probe", uart_path, parsed.config, std::make_shared<e220::TermiosUart>(),
      std::make_shared<e220::testing::FileE220Pins>(ctl_dir), timing);
  Listener listener;
  const auto started = driver->Start(listener, {});
  if (!started.HasValue()) {
    std::fprintf(stderr, "Start: %s\n", started.Error().SupportData().c_str());
    return 1;
  }
  const auto regs = driver->last_registers();
  if (mode == "start") {
    std::printf("{\"registers\": [%u, %u, %u, %u, %u, %u]}\n", regs[0], regs[1], regs[2], regs[3], regs[4], regs[5]);
  } else if (mode == "burst") {
    const int count = std::atoi(Arg(argc, argv, "--count", "20").c_str());
    const int size = std::atoi(Arg(argc, argv, "--size", "200").c_str());
    const int min_size = std::atoi(Arg(argc, argv, "--min-size", "0").c_str());
    const int max_size = std::atoi(Arg(argc, argv, "--max-size", "0").c_str());
    std::vector<std::size_t> sizes;
    {
      std::stringstream ss{Arg(argc, argv, "--sizes", "")};
      std::string item;
      while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
          sizes.push_back(static_cast<std::size_t>(std::atoi(item.c_str())));
        }
      }
    }
    std::uint32_t lcg = 12345;
    std::uint64_t sent = 0;
    std::uint64_t fnv = 1469598103934665603ULL;
    int failed = 0;
    const int frames = sizes.empty() ? count : static_cast<int>(sizes.size());
    for (int i = 0; i < frames; ++i) {
      std::size_t n = static_cast<std::size_t>(size);
      if (!sizes.empty()) {
        n = sizes[static_cast<std::size_t>(i)];
      } else if (max_size > min_size) {
        lcg = lcg * 1103515245U + 12345U;
        n = static_cast<std::size_t>(min_size) + (lcg >> 8) % static_cast<std::size_t>(max_size - min_size + 1);
      }
      std::vector<std::uint8_t> frame(n);
      for (std::size_t k = 0; k < frame.size(); ++k) {
        frame[k] = static_cast<std::uint8_t>((i * 7 + k) & 0xFF);
      }
      bool accepted = false;
      const auto deadline = Clock::now() + std::chrono::seconds(60);
      while (!accepted && Clock::now() < deadline) {
        const int seen = listener.tx_ready();
        const auto r = driver->Send(frame);
        if (r.HasValue()) {
          accepted = true;
        } else if (ara::com::mavlink::IsTransportError(r.Error(), ara::com::mavlink::TransportErrc::kBusy)) {
          if (!listener.WaitTxReady(seen, std::chrono::seconds(20))) {
            break;
          }
        } else {
          break;
        }
      }
      if (!accepted) {
        ++failed;
      } else {
        sent += n;
        for (const auto b : frame) {
          fnv = (fnv ^ b) * 1099511628211ULL;
        }
      }
    }
    const bool drained = driver->WaitTxQueueEmpty(std::chrono::seconds(60));
    std::printf("{\"sent\": %" PRIu64 ", \"failed_writes\": %d, \"drained\": %s, \"reg2\": %u, \"fnv\": \"%016" PRIx64
                "\"}\n", sent, failed, drained ? "true" : "false", regs[2], fnv);
  } else {  // sink
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    std::printf("{\"received\": %" PRIu64 ", \"fnv\": \"%016" PRIx64 "\"}\n", listener.total(), listener.fnv());
  }
  std::fflush(stdout);
  driver->Stop();
  return 0;
}
