/**
 * @file rs485.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/pd-33x/rs485/rs485.hpp"

namespace srp {
namespace core {
namespace {
    constexpr auto ON = 1;
    constexpr auto OFF = 0;
}

bool RS485::Init(const RS485_conf_t& config, std::unique_ptr<uart::IUartDriver>&& uart,
                                            std::unique_ptr<srp::gpio::IGPIOController>&& gpio) {
    this->uart_ = std::move(uart);
    this->gpio_ = std::move(gpio);
    pin_id = config.pin_id;
    return this->uart_->Open(config.port_name, config.baudrate);
}
std::optional<std::vector<uint8_t>> RS485::WriteRead(const std::vector<uint8_t>& data, const uint8_t read_size) {
    if (gpio_->SetPinValue(pin_id, ON) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    if (this->uart_->Write(data) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    if (gpio_->SetPinValue(pin_id, OFF) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    return this->uart_->Read(read_size);
}
RS485::~RS485() {
    this->uart_->Close();
    this->uart_.reset();
    this->gpio_.reset();
}
}  // namespace core
}  // namespace srp
