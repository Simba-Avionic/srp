/**
 * @file gpio_mw.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <utility>

#include "gpio_mw.hpp"
#include "mw/gpio_server/data/header.hpp"
#include "core/json/json_parser.h"
#include "core/gpio/IGPIO_digital_driver.h"
#include "core/logger/Logger.h"

using json = nlohmann::json;

namespace simba {
namespace mw {

namespace {
    constexpr auto FILE_PATH = "/opt/gpio/etc/config.json";
    constexpr auto SOCKET_PATH = "SIMBA.GPIO";
}

core::ErrorCode GPIOMWService::Init(std::unique_ptr<com::soc::ISocketStream> socket,
                              std::unique_ptr<core::gpio::IgpioDigitalDriver> gpio) {
  if (!socket || !gpio) {
    return core::ErrorCode::kInitializeError;
  }
  this->sock_ = std::move(socket);
  this->gpio_driver_ = std::move(gpio);
  return core::ErrorCode::kOk;
}

std::vector<uint8_t> GPIOMWService::RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data) {
    gpio::Header hdr(0, 0, gpio::ACTION::GET);
    hdr.SetBuffor(data);
    auto it = this->config.find(hdr.GetActuatorID());
    if (it == this->config.end()) {
        AppLogger::Warning("Unknown pin with ID: "+std::to_string(hdr.GetActuatorID()));
        return {0};
    }
    if (hdr.GetAction() == gpio::ACTION::SET) {
        if (it->second.direction != core::gpio::direction_t::OUT) {
            AppLogger::Warning("Try to set IN pin value, ID: "+std::to_string(hdr.GetActuatorID()));
            return {0};
        }
        if (this->gpio_driver_->setValue(it->second.pinNum, hdr.GetValue()) == core::ErrorCode::kOk) {
            return {1};
        }
        return {0};
    } else if (hdr.GetAction() == gpio::ACTION::GET) {
        auto val = this->gpio_driver_->getValue(it->second.pinNum);
        gpio::Header resHeader(hdr.GetActuatorID(), val, gpio::ACTION::RES);
        return resHeader.GetBuffor();
    } else {
        return {};
    }
}

core::ErrorCode GPIOMWService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}

core::ErrorCode GPIOMWService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->Init(std::make_unique<com::soc::StreamIpcSocket>(), std::make_unique<core::gpio::GpioDigitalDriver>());
    this->sock_->Init({SOCKET_PATH, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));
    std::ifstream file(FILE_PATH);
    if (!file.is_open()) {
    AppLogger::Warning("Cant find file on path /opt/gpio/etc/config.json");
    return core::ErrorCode::kError;
    }
    json data = json::parse(file);
    auto res = this->ReadConfig(data);
    if (!res.has_value()) {
        AppLogger::Warning("Cant read Config");
        return core::ErrorCode::kInitializeError;
    }
    this->config = res.value();
    this->InitPins();
    this->sock_->StartRXThread();
    return core::ErrorCode::kOk;
}
std::optional<std::unordered_map<uint8_t, GpioConf>> GPIOMWService::ReadConfig(nlohmann::json data) {
    std::unordered_map<uint8_t, GpioConf> db;
    if (!data.contains("gpio")) {
        AppLogger::Warning("cant find config file");
        return {};
    }
    if (!data.at("gpio").is_array()) {
        AppLogger::Warning("cant find config file");
        return {};
    }
    for (const auto& gpio : data["gpio"]) {
        if (!gpio.contains("id") || !gpio.contains("num") || !gpio.contains("direction")) {
            continue;
        }
        if (!gpio.at("id").is_number_unsigned() ||
                !gpio.at("num").is_number_unsigned() || !gpio.at("direction").is_string()) {
            continue;
        }
        uint8_t pin_id = static_cast<uint8_t>(gpio["id"]);
        uint16_t pin_num = static_cast<uint16_t>(gpio["num"]);
        const std::string direct = gpio.at("direction");
        core::gpio::direction_t direction = direct == "out" ?
            core::gpio::direction_t::OUT : core::gpio::direction_t::IN;
        db.insert({pin_id, {pin_num, direction}});
    }
    return db;
}

core::ErrorCode GPIOMWService::InitPins() {
    auto res = core::ErrorCode::kOk;
    for (auto pin : this->config) {
        auto r = this->gpio_driver_->initializePin(pin.second.pinNum, pin.second.direction);
        if (r != core::ErrorCode::kOk) {
            AppLogger::Warning("Cant Initialize pin with actuator_ID" + std::to_string(pin.first));
            res = r;
        }
    }
    return res;
}


}  // namespace mw
}  // namespace simba
