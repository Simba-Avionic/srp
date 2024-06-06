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
            AppLogger::Debug("Change pin with ID:" + std::to_string(it->first)
                                                    + ", to value:" + std::to_string(hdr.GetValue()));
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
std::optional<nlohmann::json> GPIOMWService::ReadJsonFromFile(std::string file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
    AppLogger::Warning("Cant find file on path /opt/gpio/etc/config.json");
    return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string str = buffer.str();
    if (!json::accept(str)) {
        AppLogger::Warning(" Invalid JSON format");
        return {};
    }
    return json::parse(str);
}

core::ErrorCode GPIOMWService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->Init(std::make_unique<com::soc::StreamIpcSocket>(), std::make_unique<core::gpio::GpioDigitalDriver>());
    this->sock_->Init({SOCKET_PATH, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));

    auto json = ReadJsonFromFile("/opt/" + parms.at("app_name") + "/etc/config.json");
    if (!json.has_value()) {
        return core::ErrorCode::kError;
    }
    auto res = this->ReadConfig(json.value());
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
    auto parser = core::json::JsonParser::Parser(data);
    if (!parser.has_value()) {
        return {};
    }
    auto par_val = parser.value();
    auto gpios = par_val.GetArray("gpio");
    if (!gpios.has_value()) {
        return {};
    }
    for (const auto& gpio : gpios.value()) {
        auto p = core::json::JsonParser::Parser(gpio);
        if (!p.has_value()) {
            AppLogger::Warning("skip invalid entity in config");
            continue;
        }
        auto pin_id = p.value().GetNumber<uint8_t>("id");
        auto pin_num = p.value().GetNumber<uint16_t>("num");
        auto str_direction = p.value().GetString("direction");
        if (!pin_id.has_value() || !pin_num.has_value() || !str_direction.has_value()) {
            continue;
        }
        auto direction = str_direction.value() == "out" ? core::gpio::direction_t::OUT : core::gpio::direction_t::IN;
        db.insert({pin_id.value(), {pin_num.value(), direction}});
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
