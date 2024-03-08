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

#include "gpio_mw.hpp"
#include "mw/gpio_server/data/header.hpp"
#include "mw/gpio_server/data/resHeader.hpp"
#include "core/json/json_parser.h"
#include "core/gpio/IGPIO_digital_driver.h"
#include "core/logger/Logger.h"

namespace simba {
namespace mw {

void GPIOMWService::RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data) {
    if (data.size() <= 0) {
        return;
    }
    gpio::Header hdr(0, 0, 0);
    hdr.SetBuffor(data);
    auto it = this->config.find(hdr.GetPinID());
    if (it == this->config.end()) {
        AppLogger::Warning("Unknown pin with ID: "+std::to_string(hdr.GetPinID()));
        return;
    }
    if (it->second.direction != core::gpio::direction_t::OUT) {
        AppLogger::Warning("Try to set IN pin value, ID: "+std::to_string(hdr.GetPinID()));
    }
    if (this->gpio_driver_.getValue(it->second.pinNum) != hdr.GetValue()) {
        if (this->gpio_driver_.setValue(it->second.pinNum, hdr.GetValue()) == core::ErrorCode::kOk) {
            gpio::ResHeader resHdr(hdr.GetPinID(), hdr.GetValue());
            this->sock_.Transmit("SIMBA.GPIO.SET."+std::to_string(hdr.GetServiceID()), 0, resHdr.GetBuffor());
        } else {
            gpio::ResHeader resHdr(hdr.GetPinID(), !hdr.GetValue());
            this->sock_.Transmit("SIMBA.GPIO.SET."+std::to_string(hdr.GetServiceID()), 0, resHdr.GetBuffor());
        }
    }
}

core::ErrorCode GPIOMWService::Run(std::stop_token token) {
    while (true) {
        this->gpio_driver_.setValue(21, 0);
        this->gpio_driver_.setValue(22, 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->gpio_driver_.setValue(21, 1);
        this->gpio_driver_.setValue(22, 0);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

core::ErrorCode GPIOMWService::Initialize(
      const std::unordered_map<std::string, std::string>& parms) {
        this->sock_.Init({"SIMBA.GPIO.SET", 0, 0});
        this->sock_.SetRXCallback(std::bind(&GPIOMWService::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
        this->InitializePins();
        this->sock_.StartRXThread();
}
void GPIOMWService::InitializePins() {
    // TODO (matikrajek42@gmail.com) READ config file   // NOLINT
    this->config.insert({1, GpioConf{21, core::gpio::direction_t::OUT}});
    this->config.insert({2, GpioConf{22, core::gpio::direction_t::OUT}});
    this->config.insert({3, GpioConf{23, core::gpio::direction_t::OUT}});
    this->config.insert({4, GpioConf{24, core::gpio::direction_t::OUT}});

    for (auto pin : this->config) {
        this->gpio_driver_.initializePin(pin.second.pinNum, pin.second.direction);
    }
}


}  // namespace mw
}  // namespace simba
