/**
 * @file sim_gpio_driver.cpp
 * @brief Sim GPIO ze stanem pin->wartosc, dziennikiem i usterkami (core/sim/sim_ctl.hpp).
 *
 * setValue zapisuje (state/gpio/<pin>, events.jsonl), getValue czyta (domyslnie 0).
 * Polecenia ctl gpio: patrz docs/audit/sim_drivers.md.
 *
 * @copyright Copyright (c) 2025
 */
#include <chrono>  // NOLINT
#include <map>
#include <mutex>  // NOLINT
#include <thread>  // NOLINT

#include "ara/log/log.h"
#include "core/file/file.hpp"
#include "core/gpio/gpio_driver.hpp"
#include "core/sim/sim_ctl.hpp"

namespace srp {
namespace core {
namespace gpio {
namespace {
constexpr char kGpioPath[] = "/sys/class/gpio";

struct Pin {
  uint8_t value{0};
  direction_t dir{direction_t::IN};
};

std::mutex g_m;
std::map<uint16_t, Pin> g_pins;

enum class Op { kSet, kGet };

// Stosuje settery z ctl i zwraca (usterka, opoznienie ms) dla operacji na pinie.
bool Sync(uint16_t pin, Op op, int64_t* delay_ms) {
  bool fault = false;
  *delay_ms = 0;
  sim::Ctl::Get().Visit([&](sim::Cmd& c) {
    if (c.tok.size() < 3 || c.tok[0] != "gpio") {
      return;
    }
    try {
      const uint16_t p = static_cast<uint16_t>(std::stoi(c.tok[1], nullptr, 0));
      const std::string& what = c.tok[2];
      if (what == "value" && c.tok.size() >= 4) {
        if (!c.applied) {
          c.applied = true;
          g_pins[p].value = static_cast<uint8_t>(std::stoi(c.tok[3]));
          sim::WriteState("gpio/" + std::to_string(p), std::to_string(g_pins[p].value));
          sim::Event("gpio", "\"op\":\"stimulus\",\"pin\":" + std::to_string(p) +
                                 ",\"val\":" + c.tok[3]);
        }
      } else if (p == pin && what == "fault" && c.tok.size() >= 4) {
        const std::string& k = c.tok[3];
        if (!fault && (k == "any" || (k == "set" && op == Op::kSet) || (k == "get" && op == Op::kGet)) &&
            sim::Consume(&c)) {
          fault = true;
          sim::Event("gpio", "\"op\":\"fault\",\"pin\":" + std::to_string(p) + ",\"kind\":\"" + k + "\"");
        }
      } else if (p == pin && what == "delay" && c.tok.size() >= 4) {
        *delay_ms = std::stoll(c.tok[3]);
      }
    } catch (...) {
      ara::log::LogWarn() << "sim gpio: bad ctl line: " << c.text;
    }
  });
  return fault;
}
}  // namespace

GpioDriver::GpioDriver(std::unique_ptr<IFileHandler> file):file_(std::move(file)),
gpio_logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("GPIO", "", ara::log::LogLevel::kWarn)} {}

GpioDriver::~GpioDriver() {}

core::ErrorCode  GpioDriver::unregisterPin(const uint16_t& pinNumber, bool /*use_lock*/) {
    sim::Event("gpio", "\"op\":\"unregister\",\"pin\":" + std::to_string(pinNumber));
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::initializePin(const uint16_t& pinNumber, const direction_t& direction,
                                          bool /*use_lock*/) {
    std::lock_guard<std::mutex> l(g_m);
    int64_t d;
    Sync(pinNumber, Op::kSet, &d);
    g_pins[pinNumber].dir = direction;
    sim::Event("gpio", "\"op\":\"init\",\"pin\":" + std::to_string(pinNumber) +
                           ",\"dir\":\"" + ((direction == direction_t::IN) ? "in" : "out") + "\"");
    return core::ErrorCode::kOk;
}

std::string GpioDriver::getEndpointPath(const uint16_t& pinNumber, const std::string& endpoint) {
    return std::string(kGpioPath) + "/gpio" + std::to_string(pinNumber) + "/" + endpoint;
}

core::ErrorCode GpioDriver::setValue(const uint16_t& pinNumber , const uint8_t& value,
                                     bool /*use_lock*/) {
    int64_t delay = 0;
    bool fault;
    {
        std::lock_guard<std::mutex> l(g_m);
        fault = Sync(pinNumber, Op::kSet, &delay);
    }
    if (delay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    if (fault) {
        return core::ErrorCode::kError;
    }
    std::lock_guard<std::mutex> l(g_m);
    g_pins[pinNumber].value = value;
    sim::WriteState("gpio/" + std::to_string(pinNumber), std::to_string(value));
    sim::Event("gpio", "\"op\":\"set\",\"pin\":" + std::to_string(pinNumber) +
                           ",\"val\":" + std::to_string(value));
    return core::ErrorCode::kOk;
}

core::ErrorCode GpioDriver::setDirection(const uint16_t& pinNumber , const direction_t& direction,
                                         bool /*use_lock*/) {
    std::lock_guard<std::mutex> l(g_m);
    int64_t d;
    if (Sync(pinNumber, Op::kSet, &d)) {
        return core::ErrorCode::kError;
    }
    g_pins[pinNumber].dir = direction;
    sim::Event("gpio", "\"op\":\"dir\",\"pin\":" + std::to_string(pinNumber) +
                           ",\"dir\":\"" + ((direction == direction_t::IN) ? "in" : "out") + "\"");
    return ErrorCode::kOk;
}

uint8_t GpioDriver::getValue(const uint16_t& pinNumber, bool /*use_lock*/) {
    int64_t delay = 0;
    bool fault;
    {
        std::lock_guard<std::mutex> l(g_m);
        fault = Sync(pinNumber, Op::kGet, &delay);
    }
    if (delay > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    if (fault) {
        return 0xFF;  // interfejs nie ma kodu bledu; 0xFF = blad odczytu w sim
    }
    std::lock_guard<std::mutex> l(g_m);
    return g_pins[pinNumber].value;
}

direction_t GpioDriver::getDirection(const uint16_t& pinNumber, bool /*use_lock*/) {
    std::lock_guard<std::mutex> l(g_m);
    return g_pins[pinNumber].dir;
}

}  // namespace gpio
}  // namespace core
}  // namespace srp
