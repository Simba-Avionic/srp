/**
 * @file eeproom.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_I2C_SERVICE_EEPROM_SERVICE_EEPROM_HPP_
#define MW_I2C_SERVICE_EEPROM_SERVICE_EEPROM_HPP_

#include <mutex>  // NOLINT
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>



namespace simba {
namespace i2c {

class Eeprom {
 private:
  uint16_t app_id{0};
  std::mutex *mtx = nullptr;
 public:
  explicit Eeprom(std::mutex *mtx);
  std::vector<uint8_t> Read(uint8_t address);
  std::optional<uint8_t> Read(uint8_t address, uint8_t reg);
  void Write(uint8_t address, std::vector<uint8_t> data);
  void Write(uint8_t address, uint8_t reg, uint8_t data);
};
}  // namespace i2c
}  // namespace simba


#endif  // MW_I2C_SERVICE_EEPROM_SERVICE_EEPROM_HPP_
