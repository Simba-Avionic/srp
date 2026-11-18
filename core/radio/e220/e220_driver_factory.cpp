/**
 * Copyright (c) 2024
 *
 * @file e220_driver_factory.cpp
 */
#include "core/radio/e220/e220_driver_factory.hpp"

#include <mutex>  // NOLINT
#include <tuple>
#include <utility>

#include "core/radio/e220/e220_driver_config.hpp"
#include "core/radio/e220/sysfs_e220_pins.hpp"
#include "core/radio/e220/termios_uart.hpp"
#include "core/radio/e220/testing/file_e220_pins.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

namespace {
/// Jedna proba zbudowania drivera z pliku configu (blad configu = driver z bledem, nie nullptr).
std::shared_ptr<E220RadioDriver> BuildE220Driver(const std::string& config_path, const E220FactoryOptions& options) {
  const std::string name = kE220DriverId;
  if (config_path.empty()) {
    return E220RadioDriver::WithConfigError(
        name, "brak pliku konfiguracji drivera (ustaw 'driver_config' lacza w configu daemona)");
  }
  const auto parsed = LoadDriverConfig(config_path);
  if (!parsed.ok) {
    return E220RadioDriver::WithConfigError(name, "konfiguracja drivera: " + parsed.error);
  }
  const auto& cfg = parsed.config;
  std::shared_ptr<IE220Pins> pins;
  if (!cfg.pins.ctl_dir.empty()) {
    if (!options.allow_ctl_dir) {
      return E220RadioDriver::WithConfigError(
          name, "plik '" + config_path + "': pins.ctl_dir jest dostepne tylko w pluginie testowym; podaj numery "
                "GPIO (pins.m0, pins.m1, pins.aux)");
    }
    pins = std::make_shared<testing::FileE220Pins>(cfg.pins.ctl_dir);
  } else {
    pins = std::make_shared<SysfsE220Pins>(cfg.pins.m0, cfg.pins.m1, cfg.pins.aux, options.sysfs_gpio_root);
  }
  return std::make_shared<E220RadioDriver>(name + "(" + cfg.uart + ")", cfg.uart, cfg.radio,
                                           std::make_shared<TermiosUart>(), std::move(pins), options.timing);
}

/// Driver dla daemona: plik configu jest wczytywany przy KAZDYM Start(), wiec poprawiony plik dziala po
/// ponownym starcie lacza (daemon ponawia Start() po `restart_delay_ms`, bez restartu procesu).
class ReloadingE220Driver final : public ara::com::mavlink::IRadioDriver {
 public:
  ReloadingE220Driver(std::string config_path, E220FactoryOptions options)
      : config_path_{std::move(config_path)}, options_{std::move(options)} {}

  ara::core::Result<void> Start(ara::com::mavlink::IRadioListener& listener,  // NOLINT(runtime/references)
                                std::stop_token token) noexcept override {
    std::shared_ptr<E220RadioDriver> inner;
    try {
      inner = BuildE220Driver(config_path_, options_);
    } catch (...) {
      return ara::com::mavlink::MakeTransportError(ara::com::mavlink::TransportErrc::kPersistent,
                                                   "e220: nie udalo sie zbudowac drivera z '" + config_path_ + "'");
    }
    {
      std::lock_guard<std::mutex> lock{mutex_};
      inner_ = inner;  // przed Start(): Stop() moze przerwac trwajaca konfiguracje modemu
    }
    return inner->Start(listener, token);
  }
  void Stop() noexcept override {
    if (const auto inner = Inner()) {
      inner->Stop();
    }
  }
  ara::core::Result<void> Send(std::span<const std::uint8_t> frame) noexcept override {
    if (const auto inner = Inner()) {
      return inner->Send(frame);
    }
    return ara::com::mavlink::MakeTransportError(ara::com::mavlink::TransportErrc::kClosed,
                                                 "e220: driver nie wystartowal");
  }
  std::string Name() const noexcept override {
    std::lock_guard<std::mutex> lock{mutex_};
    return inner_ ? inner_->Name() : std::string("e220:") + kE220DriverId;
  }

 private:
  std::shared_ptr<E220RadioDriver> Inner() const {
    std::lock_guard<std::mutex> lock{mutex_};
    return inner_;
  }
  const std::string config_path_;
  const E220FactoryOptions options_;
  mutable std::mutex mutex_;
  std::shared_ptr<E220RadioDriver> inner_;
};
}  // namespace

std::shared_ptr<ara::com::mavlink::IRadioDriver> CreateE220Driver(const std::string& config_path,
                                                                 const E220FactoryOptions& options) {
  return std::make_shared<ReloadingE220Driver>(config_path, options);
}

void RegisterE220Driver(ara::com::mavlink::IRadioDriverRegistry& registry,  // NOLINT(runtime/references)
                        E220FactoryOptions options) {
  std::ignore = registry.Register(kE220DriverId, [options](const std::string& config_path) {
    return CreateE220Driver(config_path, options);
  });
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
