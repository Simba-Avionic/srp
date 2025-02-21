/**
 * @file pd33x.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "core/pd_33x/pd33x.hpp"
#include <cstring>
namespace srp {
namespace core {
namespace {
    constexpr auto kUart_path = "/dev/ttyS1";
    constexpr auto kUart_baudrate = B1152000;
    constexpr auto kTransreceived_pin_id = 12;
    constexpr auto ON = 1;
    constexpr auto OFF = 0;
    constexpr auto kReadSize = 9;
}

uint16_t RS485::Generate_CRC16(const std::vector<uint8_t>& data) {
    unsigned int Crc = 0xFFFF;
    unsigned char n, m, x;
    m = data.size();
    x = 0;
    while (m > 0) {
        Crc^= data[x];
        for (n=0; n < 8; n++) {
            if (Crc&1) {
                Crc>>= 1;
                Crc^= 0xA001;
            } else {
                Crc>>= 1;
            }
        }
        m--;
        x++;
    }
    return Crc;
}
std::vector<uint8_t> RS485::add_CRC(std::vector<uint8_t> data) {
    auto crc = Generate_CRC16(data);
    // mozliwe ze crc trzeba zamienic kolejnosc LSB /MSB
    data.push_back(crc & 0xFF);
    data.push_back((crc >> 8) & 0xFF);
    return data;
}
bool RS485::Init(std::unique_ptr<uart::IUartDriver> uart, std::unique_ptr<gpio::IGPIOController> gpio) {
    uart_ = std::move(uart);
    gpio_ = std::move(gpio);
    this->uart_->Open(kUart_path, kUart_baudrate);
}
std::optional<std::vector<uint8_t>> RS485::WriteRead(const std::vector<uint8_t>& data, const uint8_t read_size) {
    if (gpio_->SetPinValue(kTransreceived_pin_id, ON) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    if (this->uart_->Write(add_CRC(data)) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    if (gpio_->SetPinValue(kTransreceived_pin_id, OFF) != core::ErrorCode::kOk) {
        return std::nullopt;
    }
    return this->uart_->Read(read_size);
}
float RS485::parseFloat(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> reorderedData = {data[3], data[2], data[1], data[0]};  // Adjust for IEEE754 format
    float value;
    std::memcpy(&value, reorderedData.data(), sizeof(float));
    return value;
}

std::optional<d_press_t> RS485::ReadTempAndPress() {
    const std::vector<uint8_t> data = {0x01, 0x03, 0x01, 0x00, 0x00, 0x04};
    auto res = WriteRead(data, 13);
    if (!res.has_value()) {
        return std::nullopt;
    }
    const std::vector<uint8_t> firstSubset(res.value().begin() + 3, res.value().begin() + 6);
    const std::vector<uint8_t> secondSubset(res.value().begin() + 7, res.value().begin() + 10);
    d_press_t d;
    d.d_press = parseFloat(firstSubset);
    d.temp = parseFloat(secondSubset);
    return d;
}

}  // namespace core
}  // namespace srp
