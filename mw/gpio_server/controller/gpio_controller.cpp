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
    std::optional<srp::mw::gpio::GpioHdr> hdr = srp::data::Convert<srp::mw::gpio::GpioHdr>::Conv(data);
    if (!hdr.has_value()) {
        ara::log::LogWarn() << "controller ID: " << id << " received malformed callback payload";
        return {0};
    }
    auto pin_id = hdr.value().pin_id;
    if (hdr.value().action != gpio::ACTION::CALLBACK) return {0};
    if (!this->callback.has_value() || subsbribed_pins.find(pin_id) == subsbribed_pins.end()) {
        return {1};
    }
    ara::log::LogDebug() << "controller ID: " << id << " callback pin " << pin_id
                         << " value " << hdr.value().value;
    this->callback.value()(pin_id, hdr.value().value);
    return {1};
}

core::ErrorCode GPIOController::SetPinValue(uint8_t actuatorID, int8_t value,
                                            const uint16_t active_time, const bool force_time) {
    if (this->sock_== nullptr) {
        ara::log::LogError() << "SetPinValue failed: socket not initialized";
        return core::ErrorCode::kInitializeError;
    }
    srp::mw::gpio::GpioHdr hdr {gpio::ACTION::SET, actuatorID,
                                static_cast<uint8_t>(value), active_time, force_time};
    auto buf = srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr);
    auto res = this->sock_->Transmit(PATH, 0, buf);
    if (res.has_value()) {
      if (res.value().empty()) {
          ara::log::LogWarn() << "SetPinValue failed: empty response for actuator ID "
                              << actuatorID;
          return core::ErrorCode::kConnectionError;
      }
      const auto ret = static_cast<core::ErrorCode>(res.value()[0]);
      if (ret != core::ErrorCode::kOk) {
          ara::log::LogWarn() << "SetPinValue failed for actuator ID " << actuatorID
                              << ", code: " << ret;
      }
      return ret;
    }
    ara::log::LogError() << "SetPinValue failed: transport error for actuator ID "
                         << actuatorID;
    return core::ErrorCode::kConnectionError;
}

std::optional<int8_t> GPIOController::GetPinValue(uint8_t actuatorID) {
    if (this->sock_ == nullptr) {
        ara::log::LogError() << "GetPinValue failed: socket not initialized";
        return std::nullopt;
    }
    srp::mw::gpio::GpioHdr hdr {gpio::ACTION::GET, actuatorID, 0};
    auto buf = srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr);
    auto res = this->sock_->Transmit(PATH, 0, buf);
    if (!res.has_value()) {
        ara::log::LogWarn() << "GetPinValue transport error for actuator ID "
                            << actuatorID;
        return std::nullopt;
    }
    auto resHdr = srp::data::Convert<srp::mw::gpio::GpioHdr>::Conv(res.value());
    if (!resHdr.has_value()) {
        ara::log::LogWarn() << "GetPinValue decode error for actuator ID "
                            << actuatorID;
        return std::nullopt;
    }
    if (resHdr.value().action != gpio::ACTION::RES || resHdr.value().pin_id != actuatorID) {
        ara::log::LogWarn() << "GetPinValue invalid response for actuator ID "
                            << actuatorID
                            << " (action=" << resHdr.value().action
                            << ", pin_id=" << resHdr.value().pin_id << ")";
        return std::nullopt;
    }
    return resHdr.value().value;
}

core::ErrorCode GPIOController::ManagePinSubscription(const uint8_t pin_id, const bool subscribe) {
    ara::log::LogInfo() << "controller ID: " << id << " " << (subscribe ? "subscribing" : "unsubscribing")
                        << " to pin ID: " << pin_id;
    if (this->sock_ == nullptr) {
        ara::log::LogError() << "ManagePinSubscription failed: socket not initialized";
        return core::ErrorCode::kInitializeError;
    }
    bool already_subscribed = subsbribed_pins.find(pin_id) != subsbribed_pins.end();
    if (already_subscribed == subscribe) {
        ara::log::LogDebug() << "ManagePinSubscription no-op for pin ID "
                             << pin_id;
        return core::ErrorCode::kOk;
    }
    auto action = subscribe ? gpio::ACTION::SUBSCRIBE : gpio::ACTION::UNSUBSCRIBE;
    srp::mw::gpio::GpioHdr hdr {action, pin_id, id};
    auto buf = srp::data::Convert2Vector<srp::mw::gpio::GpioHdr>::Conv(hdr);
    auto res = this->sock_->Transmit(PATH, 0, buf);
    if (!res.has_value()) {
        ara::log::LogWarn() << "ManagePinSubscription failed: transport error for pin ID "
                            << pin_id;
        return core::ErrorCode::kConnectionError;
    }
    if (res.value().empty()) {
        ara::log::LogWarn() << "ManagePinSubscription failed: empty response for pin ID "
                            << pin_id;
        return core::ErrorCode::kConnectionError;
    }
    if (res.value()[0] == 0) {
        ara::log::LogWarn() << "ManagePinSubscription rejected by service for pin ID "
                            << pin_id;
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
