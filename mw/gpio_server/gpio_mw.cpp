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

core::ErrorCode GPIOMWService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
    this->Init(std::make_unique<com::soc::StreamIpcSocket>(), std::make_unique<core::gpio::GpioDigitalDriver>());
    this->sock_->Init({SOCKET_PATH, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));
    const std::string path = "/opt/" + parms.at("app_name") + "/etc/config.json";
    auto config_opt = ReadConfig(path);
    if (!config_opt.has_value()) {
        AppLogger::Error("fail to read config");
        exit(1);
        return core::ErrorCode::kError;
    }
    config = config_opt.value();
    this->InitPins();
    this->sock_->StartRXThread();
    return core::ErrorCode::kOk;
}
std::optional<std::unordered_map<uint8_t, GpioConf>> GPIOMWService::ReadConfig(
      std::string path) const {
    auto parser_opt = core::json::JsonParser::Parser(path);
    if (!parser_opt.has_value()) {
        return std::nullopt;
    }
    std::unordered_map<uint8_t, GpioConf> db;
    auto parser = parser_opt.value();
    auto gpio_opt = parser.GetArray("gpio");
    if (!gpio_opt.has_value()) {
        return std::nullopt;
    }
    for (const auto & data : gpio_opt.value()) {
        auto parser_opt = core::json::JsonParser::Parser(data);
        if (!parser_opt.has_value()) {
            continue;
        }
        auto parser = parser_opt.value();

        auto pin_id_opt = parser.GetNumber<uint8_t>("id");
        auto pin_num_opt = parser.GetNumber<uint16_t>("num");
        auto str_direction_opt = parser.GetString("direction");
        if (!(pin_id_opt.has_value() && pin_num_opt.has_value() && str_direction_opt.has_value())) {
            continue;
        }
        auto direction = str_direction_opt.value() == "out" ?
                                    core::gpio::direction_t::OUT : core::gpio::direction_t::IN;
        db.insert({pin_id_opt.value(), {pin_num_opt.value(), direction}});
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
