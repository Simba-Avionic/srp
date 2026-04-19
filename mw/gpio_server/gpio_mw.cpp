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
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <utility>
#include <sstream>
#include<algorithm>

#include "gpio_mw.hpp"
#include "mw/gpio_server/data/enums.hpp"
#include "core/json/json_parser.h"
#include "core/common/condition.h"
#include "core/gpio/Igpio_driver.hpp"
#include "ara/log/log.h"
using json = nlohmann::json;

namespace srp {
namespace mw {

namespace {
    static constexpr auto SOCKET_PATH = "SRP.GPIO";
    static constexpr auto CALLBACK_PATH_PREFIX = "SRP.GPIO.";
    static constexpr auto AUTO_DISABLE_PIN_DELAY = 100;
    static constexpr auto STATE_POLL_DELAY = std::chrono::milliseconds(100);
    static constexpr auto kDID_instance_name = "/srp/mw/gpio_service/gpio_pin_did";
}

int GPIOMWService::Init(std::unique_ptr<srp::com::soc::ISocketStream> socket,
                              std::shared_ptr<core::gpio::IGpioDriver> gpio) {
  if (!socket || !gpio) {
    return 1;
  }
  this->sock_ = std::move(socket);
  this->gpio_driver_ = std::move(gpio);
  return 0;
}
GPIOMWService::GPIOMWService():did_instance(kDID_instance_name) {
}

std::vector<uint8_t> GPIOMWService::RxCallback(const std::string& ip, const std::uint16_t& port,
  const std::vector<std::uint8_t> data) {
    std::optional<srp::mw::gpio::GpioHdr> hdr = srp::data::Convert<srp::mw::gpio::GpioHdr>::Conv(data);
    if (!hdr.has_value()) {
        return {0};
    }
    auto it = this->config.find(hdr.value().pin_id);
    if (it == this->config.end()) {
        ara::log::LogWarn() << "Unknown pin with ID: " << std::to_string(hdr.value().pin_id);
        return {0};
    }
    auto action = hdr.value().action;
    switch (action) {
        case srp::gpio::ACTION::SET: {
            if (it->second.direction != core::gpio::direction_t::OUT) {
                ara::log::LogWarn() << ("Try to set IN pin value, ID: " + std::to_string(hdr.value().pin_id));
                return {core::ErrorCode::kError};
            }
            if (this->gpio_driver_->setValue(it->second.pinNum, hdr.value().value) != core::ErrorCode::kOk) {
                return {core::ErrorCode::kError};
            }
            ara::log::LogDebug() << ("Change pin with ID:" + std::to_string(it->first) +
                                        ", to value:" + std::to_string(hdr.value().value) +
                                            "for: " + std::to_string(hdr.value().time_period) + "s");
            if (hdr.value().value == 0) {
                return {core::ErrorCode::kOk};
            }
            auto it = pin_expire.find(hdr.value().pin_id);
            if (it == pin_expire.end()) {
                ExpiredPinCB cfg;
                cfg.disable_tp = std::chrono::high_resolution_clock::now()
                                        + std::chrono::milliseconds(hdr.value().time_period);
                cfg.infinite_active = (hdr.value().time_period == 0);
                pin_expire[hdr.value().pin_id] =  cfg;
                return {core::ErrorCode::kOk};
            }
            auto new_time = std::chrono::high_resolution_clock::now()
                                    + std::chrono::milliseconds(hdr.value().time_period);
            it->second.disable_tp = std::max(it->second.disable_tp, new_time);
            if (hdr.value().time_period == 0) {
                it->second.infinite_active = true;
            } else if (hdr.value().force_time_period) {
                it->second.infinite_active = false;
            }
            return {core::ErrorCode::kOk};
        }
        case srp::gpio::ACTION::GET: {
            auto val = this->gpio_driver_->getValue(it->second.pinNum);
            srp::mw::gpio::GpioHdr hdr2{srp::gpio::ACTION::RES, hdr.value().pin_id, val};
            return srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr2);
        }

        case srp::gpio::ACTION::SUBSCRIBE: {
            auto controller_id = hdr.value().value;
            auto pin_id = hdr.value().pin_id;
            ara::log::LogDebug() << "subscription request from controller ID: " << std::to_string(controller_id)
                                 << " for pin ID: " << std::to_string(pin_id);
            if (config.find(pin_id) == config.end()) {
                ara::log::LogWarn() << "subscription failed, unknown pin with ID: " << std::to_string(pin_id);
                return {0};  // cannot return any other value, as it would be interpreted as a controller ID
            }
            if (controller_id == 0) {
                controller_id = next_controller_id++;
            }
            if (subscribed_pins_states.find(pin_id) == subscribed_pins_states.end()) {
                subscribed_pins_states[pin_id] = 0;
            }
            callbacks[pin_id].push_back(controller_id);
            ara::log::LogInfo() << "controller ID: " << std::to_string(controller_id)
                                 << " successfully subscribed to pin ID: " << std::to_string(pin_id);
            return {controller_id};
        }

        case srp::gpio::ACTION::UNSUBSCRIBE: {
            auto controller_id = hdr.value().value;
            auto pin_id = hdr.value().pin_id;
            if (subscribed_pins_states.find(pin_id) == subscribed_pins_states.end() ||
                callbacks.find(pin_id) == callbacks.end()) {
                ara::log::LogWarn() << "unsubscription failed, unknown pin ID: " << std::to_string(pin_id);
                return {0};
            }
            auto pin_callbacks = callbacks[pin_id];
            auto find = std::find(pin_callbacks.begin(), pin_callbacks.end(), controller_id);
            if (find == pin_callbacks.end()) {
                ara::log::LogWarn() << "unsubscription failed, unknown controller ID: "
                                    << std::to_string(controller_id);
                return {0};
            }
            pin_callbacks.erase(find);
            if (pin_callbacks.empty()) {
                subscribed_pins_states.erase(pin_id);
            }
            ara::log::LogDebug() << "controller ID: " << std::to_string(controller_id)
                                 << " successfully unsubscribed from pin ID: " << std::to_string(pin_id);
            return {1};
        }

        default:
            return {};
    }
}

void GPIOMWService::PollSubscribedPinsLoop(const std::stop_token& token) {
    ara::log::LogDebug() << "Starting subscribed pins polling loop";
    for (auto pair : subscribed_pins_states) {
        // subscribed pins are checked for existence before being added to the set
        pair.second = this->gpio_driver_->getValue(config[pair.first].pinNum);
    }
    while (!token.stop_requested()) {
        for (auto& pair : subscribed_pins_states) {
            auto state = this->gpio_driver_->getValue(config[pair.first].pinNum);
            if (state == pair.second) {
                continue;
            }
            ara::log::LogDebug() << "subscribed pin ID: " << std::to_string(pair.first)
                                 << " changed state to: " << std::to_string(state);
            pair.second = state;
            for (auto controller_id : callbacks[pair.first]) {
                srp::mw::gpio::GpioHdr hdr{srp::gpio::ACTION::CALLBACK, pair.first, state};
                auto buf = srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr);
                std::string callback_path = CALLBACK_PATH_PREFIX + std::to_string(controller_id);
                auto res = this->sock_->Transmit(callback_path, 0, buf);
                ara::log::LogDebug() << "callback to controller ID: " << std::to_string(controller_id)
                                     << " for pin ID: " << std::to_string(pair.first);
                if (!res.has_value()) {
                    ara::log::LogWarn() << "callback to controller ID: " << std::to_string(controller_id) << " failed";
                }
            }
        }
        core::condition::wait_for(STATE_POLL_DELAY, token);
    }
}

GPIOMWService::~GPIOMWService() {
    this->sock_->~ISocketStream();
}

void GPIOMWService::CheckPinsExpired() {
    auto now = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(pin_expire_mutex);

    for (auto it = pin_expire.begin(); it != pin_expire.end(); ) {
        if (it->second.infinite_active) {
            ++it;
            continue;
        }
        if (now < it->second.disable_tp) {
            ++it;
            continue;
        }

        const uint8_t pin_id = it->first;
        auto pin_config = config.find(pin_id);
        if (pin_config == config.end()) {
            ++it;
            continue;
        }

        gpio_driver_->setValue(pin_config->second.pinNum, 0);
        it = pin_expire.erase(it);
    }
}

int GPIOMWService::Run(const std::stop_token& token) {
    pin_expire_thread = std::jthread([this](const std::stop_token& token){
        while (!token.stop_requested()){
            CheckPinsExpired();
            core::condition::wait_for(std::chrono::milliseconds(AUTO_DISABLE_PIN_DELAY), token);
        }
    });
    pin_did_ = std::make_unique<GpioMWDID>(this->did_instance, this->gpio_driver_, config);
    pin_did_->Offer();
    this->sock_->StartRXThread();
    PollSubscribedPinsLoop(token);
    this->sock_->StopRXThread();
    this->pin_did_->StopOffer();
    return core::ErrorCode::kOk;
}

int GPIOMWService::Initialize(const std::map<ara::core::StringView,
    ara::core::StringView> parms) {
    this->Init(std::make_unique<com::soc::StreamIpcSocket>(),
            std::make_shared<core::gpio::GpioDriver>(std::make_unique<core::FileHandler>()));
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
    return 0;
}
std::optional<std::unordered_map<uint8_t, GpioConf>> GPIOMWService::ReadConfig(
      const std::string& path) const {
    auto parser_opt = core::json::JsonParser::Parser(path);
    if (!parser_opt.has_value()) {
        return std::nullopt;
    }
    std::unordered_map<uint8_t, GpioConf> db;
    auto parser = parser_opt.value();
    auto gpio_opt = parser.GetArray<nlohmann::json>("gpio");
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
}  // namespace srp
