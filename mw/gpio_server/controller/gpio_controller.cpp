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

#include "gpio_controller.hpp"

namespace simba {
namespace gpio {

namespace {
    constexpr auto PATH = "SIMBA.GPIO";
}

GPIOController::GPIOController(std::unique_ptr<com::soc::ISocketStream> socket)
                                                    : sock_(std::move(socket)) {
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


}  // namespace gpio
}  // namespace simba
