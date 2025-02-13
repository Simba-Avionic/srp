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
#include <sstream>

#include "ara/log/log.h"
#include "gpio_controller.hpp"

namespace srp {
namespace gpio {

namespace {
    constexpr auto PATH = "SRP.GPIO";
    constexpr auto CALLBACK_PATH_PREFIX = "SRP.GPIO.";
}

GPIOController::GPIOController(std::unique_ptr<srp::com::soc::ISocketStream> socket) : sock_(std::move(socket)) {}

GPIOController::~GPIOController() {
    if (this->sock_ != nullptr) {
        this->sock_->StopRXThread();
    }
}

void GPIOController::ListenToCallbacks() {
    ara::log::LogDebug() << "controller ID: " << id << " started listening to callbacks";
    std::string path = CALLBACK_PATH_PREFIX + std::to_string(id);
    this->sock_->Init({path, 0, 0});
    this->sock_->SetRXCallback(std::bind(&GPIOController::HandleCallback, this, std::placeholders::_1,
                                         std::placeholders::_2, std::placeholders::_3));
    this->sock_->StartRXThread();
}

std::vector<uint8_t> GPIOController::HandleCallback(const std::string& _ip, const std::uint16_t& _port,
                                                    const std::vector<std::uint8_t>& data) {
    ara::log::LogDebug() << "controller ID: " << id << " received callback";
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
      return (core::ErrorCode)res.value()[0];
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

core::ErrorCode GPIOController::ManagePinSubscription(const uint8_t pin_id, const bool subscribe) {
    ara::log::LogInfo() << "controller ID: " << id << " " << (subscribe ? "subscribing" : "unsubscribing")
                        << " to pin ID: " << pin_id;
    if (this->sock_ == nullptr) {
        return core::ErrorCode::kInitializeError;
    }
    bool already_subscribed = subsbribed_pins.find(pin_id) != subsbribed_pins.end();
    if (already_subscribed == subscribe) {
        return core::ErrorCode::kOk;
    }
    auto action = subscribe ? gpio::ACTION::SUBSCRIBE : gpio::ACTION::UNSUBSCRIBE;
    gpio::Header hdr(pin_id, id, action);
    auto res = this->sock_->Transmit(PATH, 0, hdr.GetBuffor());
    if (!res.has_value()) {
        return core::ErrorCode::kConnectionError;
    }
    if (res.value()[0] == 0) {
        return core::ErrorCode::kError;
    }
    if (subscribe) {
        subsbribed_pins.insert(pin_id);
    } else {
        subsbribed_pins.erase(pin_id);
    }
    if (subscribe && id == 0) {
        id = res.value()[0];
        ara::log::LogDebug() << "controller got assigned ID: " << id;
        ListenToCallbacks();
    }
    return core::ErrorCode::kOk;
}

void GPIOController::SetCallback(const std::optional<PinChangeCallback> callback) {
    this->callback = callback;
}

}  // namespace gpio
}  // namespace srp
