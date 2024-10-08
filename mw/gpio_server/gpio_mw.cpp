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
#include <map>
#include <unordered_map>
#include <iostream>
#include <utility>

#include "gpio_mw.hpp"
#include "mw/gpio_server/data/header.hpp"
#include "core/json/json_parser.h"
#include "core/common/condition.h"
#include "core/gpio/IGPIO_digital_driver.h"
#include "ara/log/log.h"
using json = nlohmann::json;

namespace simba {
namespace mw {

namespace {
    constexpr auto SOCKET_PATH = "SIMBA.GPIO";
}

int GPIOMWService::Init(std::unique_ptr<com::soc::ISocketStream> socket,
                              std::shared_ptr<core::gpio::IgpioDigitalDriver> gpio) {
  if (!socket || !gpio) {
    return 1;
  }
  this->sock_ = std::move(socket);
  this->gpio_driver_ = std::move(gpio);
  return 0;
}

std::vector<uint8_t> GPIOMWService::RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data) {
    gpio::Header hdr(0, 0, gpio::ACTION::GET);
    hdr.SetBuffor(data);
    auto it = this->config.find(hdr.GetActuatorID());
    if (it == this->config.end()) {
        ara::log::LogWarn() << "Unknown pin with ID: " << std::to_string(hdr.GetActuatorID());
        return {0};
    }
    if (hdr.GetAction() == gpio::ACTION::SET) {
        if (it->second.direction != core::gpio::direction_t::OUT) {
            ara::log::LogWarn() <<("Try to set IN pin value, ID: "+std::to_string(hdr.GetActuatorID()));
            return {0};
        }
        if (this->gpio_driver_->setValue(it->second.pinNum, hdr.GetValue()) == core::ErrorCode::kOk) {
            ara::log::LogDebug() <<("Change pin with ID:" + std::to_string(it->first)
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
GPIOMWService::~GPIOMWService() {
    this->sock_->~ISocketStream();
}


int GPIOMWService::Run(const std::stop_token& token) {
    core::condition::wait(token);
    this->sock_->StopRXThread();
    this->pin_did_->StopOffer();
    return core::ErrorCode::kOk;
}

int GPIOMWService::Initialize(const std::map<ara::core::StringView,
    ara::core::StringView> parms) {
    this->Init(std::make_unique<com::soc::StreamIpcSocket>(), std::make_shared<core::gpio::GpioDigitalDriver>());
    this->sock_->Init({SOCKET_PATH, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3));
    const std::string path = parms.at("app_path") + "etc/config.json";
    auto config_opt = ReadConfig(path);
    if (!config_opt.has_value()) {
        ara::log::LogError() << "fail to read config";
        exit(1);
        return core::ErrorCode::kError;
    }
    config = config_opt.value();
    this->InitPins();
    pin_did_ = std::make_unique<GpioMWDID>(
                    ara::core::InstanceSpecifier("/simba/mw/gpio_service/gpio_pin_did"), this->gpio_driver_, config);
    pin_did_->StartOffer();
    this->sock_->StartRXThread();
    return 0;
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

int GPIOMWService::InitPins() {
    auto res = 0;
    for (auto pin : this->config) {
        auto r = this->gpio_driver_->initializePin(pin.second.pinNum, pin.second.direction);
        if (r != 0) {
            ara::log::LogWarn() << "Cant Initialize pin with actuator_ID" << pin.first;
            res = r;
        }
    }
    return res;
}

}  // namespace mw
}  // namespace simba
