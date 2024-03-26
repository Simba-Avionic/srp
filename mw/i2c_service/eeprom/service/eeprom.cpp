/**
 * @file eeprom.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "eeprom.hpp"

#define BUS "/dev/i2c-2"

namespace simba {
namespace i2c {

Eeprom::Eeprom(std::mutex *mtx) {
    this->mtx = mtx;
}
std::vector<uint8_t> Eeprom::Read(uint8_t address) {
    int file;
    uint8_t buffer[16];
    std::lock_guard<std::mutex> lock(*this->mtx);
    if ((file = open(BUS, O_RDWR)) < 0) {
        return {};
    }
    if (ioctl(file, I2C_SLAVE, address) < 0) {
         return {};
    }
    if (ioctl(file, I2C_RDWR, 0x00) < 0) {
         return {};
    }
    if (read(file, buffer, 16) != 16) {
         return {};
    }
    close(file);
    std::vector<uint8_t> vec{buffer, buffer + sizeof(buffer)};
    return vec;
}
std::optional<uint8_t> Eeprom::Read(uint8_t address, uint8_t reg) {
    int file;
    std::lock_guard<std::mutex> lock(*this->mtx);
    if ((file = open(BUS, O_RDWR)) < 0) {
        return {};
    }
    if (write(file, &reg, 1) != 1) {
        return {};
    }
    uint8_t data;
    if (read(file, &data, 1) != 1) {
        return {};
    }
    return data;
}

void Eeprom::Write(uint8_t address, std::vector<uint8_t> data) {
 // TODO
}
void Eeprom::Write(uint8_t address, uint8_t reg, uint8_t data) {

}

}  // namespace i2c
}  // namespace simba