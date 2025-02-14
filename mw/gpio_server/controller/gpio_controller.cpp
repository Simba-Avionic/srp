/**
 * @file gpio_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <utility>

#include "ara/log/log.h"
#include "gpio_controller.hpp"

namespace srp {
namespace gpio {

namespace {
    constexpr auto PATH = "SRP.GPIO";
}

GPIOController::GPIOController(std::unique_ptr<srp::com::soc::ISocketStream> socket)
                                                    : sock_(std::move(socket)) {
    ListenToCallbacks();
}

GPIOController::~GPIOController() { this->sock_->StopRXThread(); }

void GPIOController::ListenToCallbacks() {
    if (this->sock_ == nullptr) {
        return;
    }
    this->sock_->Init({PATH, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOController::HandleCallback, this, std::placeholders::_1,
                                         std::placeholders::_2, std::placeholders::_3));
    this->sock_->StartRXThread();
}

std::vector<uint8_t> GPIOController::HandleCallback(const std::string& _ip, const std::uint16_t& _port,
                                                    const std::vector<std::uint8_t> data) {
    gpio::Header hdr(0, 0, gpio::ACTION::GET);
    hdr.SetBuffor(data);
    auto pin_id = hdr.GetActuatorID();
    if (hdr.GetAction() != gpio::ACTION::CALLBACK) return {0};
    if (!this->callback.has_value() || subsbribed_pins.find(pin_id) == subsbribed_pins.end()) {
        return {1};
    }
    this->callback.value()(pin_id, hdr.GetValue());
    return {1};
}

core::ErrorCode GPIOController::SetPinValue(uint8_t actuatorID, int8_t value) {
    if (this->sock_== nullptr) {
        return core::ErrorCode::kInitializeError;
    }
    gpio::Header hdr(actuatorID, value, ACTION::SET);
    auto res = this->sock_->Transmit(PATH, 0, hdr.GetBuffor());
    if (res.has_value()) {
        if (res.value()[0] == 1) {
          return core::ErrorCode::kOk;
        } else {
          return core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}

std::optional<int8_t> GPIOController::GetPinValue(uint8_t actuatorID) {
    if (this->sock_ == nullptr) {
        return std::nullopt;
    }
    gpio::Header hdr(actuatorID, 0, gpio::ACTION::GET);
    auto res = this->sock_->Transmit(PATH, 0, hdr.GetBuffor());
    if (!res.has_value()) {
        return std::nullopt;
    }
    gpio::Header resHdr(0, 0, gpio::ACTION::RES);
    resHdr.SetBuffor(res.value());
    return resHdr.GetValue();
}

core::ErrorCode GPIOController::SubscribePin(const uint8_t pin_id, const bool subscribe) {
    if (this->sock_ == nullptr) {
        return core::ErrorCode::kInitializeError;
    }
    bool already_subscribed = subsbribed_pins.find(pin_id) != subsbribed_pins.end();
    if (already_subscribed == subscribe) {
        return core::ErrorCode::kOk;
    }
    if (subscribe) {
        subsbribed_pins.insert(pin_id);
    } else {
        subsbribed_pins.erase(pin_id);
    }
    auto action = subscribe ? gpio::ACTION::SUBSCRIBE: gpio::ACTION::UNSUBSCRIBE;
    gpio::Header hdr(pin_id, 0, action);
    auto res = this->sock_->Transmit(PATH, 0, hdr.GetBuffor());
    if (res.has_value()) {
        if (res.value()[0] == 1) {
          return core::ErrorCode::kOk;
        } else {
          return core::ErrorCode::kError;
        }
    }
    return core::ErrorCode::kConnectionError;
}

void GPIOController::SetCallback(const std::optional<PinChangeCallback> callback) {
    this->callback = callback;
}

}  // namespace gpio
}  // namespace srp
