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

#include "gpio_mw.hpp"
#include "mw/gpio_server/data/header.hpp"
#include "core/json/json_parser.h"
#include "core/gpio/IGPIO_digital_driver.h"
#include "core/logger/Logger.h"

using json = nlohmann::json;

namespace simba {
namespace mw {

void GPIOMWService::RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data) {
    gpio::Header hdr(0, 0, 0);
    hdr.SetBuffor(data);
    auto it = this->config.find(hdr.GetPinID());
    if (it == this->config.end()) {
        AppLogger::Warning("Unknown pin with ID: "+std::to_string(hdr.GetPinID()));
        return;
    }
    if (it->second.direction != core::gpio::direction_t::OUT) {
        AppLogger::Warning("Try to set IN pin value, ID: "+std::to_string(hdr.GetPinID()));
        return;
    }
    if (this->gpio_driver_.getValue(it->second.pinNum) != hdr.GetValue()) {
        AppLogger::Debug("set pin to position:"+std::to_string(hdr.GetValue()));
        if (this->gpio_driver_.setValue(it->second.pinNum, hdr.GetValue()) != core::ErrorCode::kOk) {
            AppLogger::Warning("pin state not change");
        }
    }
}

core::ErrorCode GPIOMWService::Run(std::stop_token token) {
    this->SleepMainThread();
    return core::ErrorCode::kOk;
}

core::ErrorCode GPIOMWService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
        this->sock_.Init({"SIMBA.GPIO.SET", 0, 0});
        this->sock_.SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        this->InitializePins();
        this->sock_.StartRXThread();
    return core::ErrorCode::kOk;
}
void GPIOMWService::InitializePins() {
    std::ifstream file("/opt/gpio/etc/config.json");
    if (!file.is_open()) {
        AppLogger::Warning("file not found");
        return;
    }
    nlohmann::json data = nlohmann::json::parse(file);
    if (!data.contains("gpio")) {
        AppLogger::Warning("cant find config file");
        return;
    }
    for (const auto& gpio : data["gpio"]) {
        std::cout<< gpio["id"] <<":"<< gpio["num"] <<std::endl;
        uint16_t pin_id = static_cast<uint16_t>(gpio["id"]);
        uint16_t pin_num = static_cast<uint16_t>(gpio["num"]);
        const std::string direct = gpio.at("direction");
        core::gpio::direction_t direction = direct == "out" ?
            core::gpio::direction_t::OUT : core::gpio::direction_t::IN;
        this->config.insert({
            pin_id, {
                pin_num, direction}
        });
    }
    for (auto pin : this->config) {
        this->gpio_driver_.initializePin(pin.second.pinNum, pin.second.direction);
    }
}


}  // namespace mw
}  // namespace simba
