/**
 * @file router.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "apps/example/router.h"

#include "core/logger/Logger.h"
namespace simba {
namespace router {

#define EEPROM_DEVICE "/dev/i2c-2"  // Ścieżka do urządzenia I2C
#define EEPROM_ADDRESS 0x50         // Adres EEPROM
#define DATA_SIZE 128

core::ErrorCode Router::Run(std::stop_token token) {
    uint8_t pcaAddress = 0x70;

    // Register addresses for PCA9685
    const uint8_t MODE1 = 0x00;
    const uint8_t PRESCALE = 0xFE;
    const uint8_t LED0_ON_L = 0x06; // LED0 ON_L register for channel 0

    // Set PWM frequency (for example, 50Hz)
    uint8_t prescaleValue = 121; // Calculation for 50Hz, you may need to adjust this
    i2c_.Write(pcaAddress, MODE1, 0x10); // Set sleep mode to allow prescale change
    i2c_.Write(pcaAddress, PRESCALE, prescaleValue); // Set prescale value for 50Hz
    i2c_.Write(pcaAddress, MODE1, 0x00); // Clear sleep mode to start PWM output

    // Set PWM duty cycle for channel 0 (for example, 50%)
    i2c_.Write(pcaAddress, LED0_ON_L, 204); // 50% duty cycle for 12-bit resolution

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto data = i2c_.Read(0x50,0x00,1);
    if (data.has_value()) {
      int i = 0;
      for (const auto x : data.value()) {
        AppLogger::Error("["+std::to_string(i)+"]:"+std::to_string(x));
        i++;
      }
    }

    // Wyświetlenie odczytanych danych
    AppLogger::Info("Odczytane dane z EEPROM:");
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->gpio_.Init(12);
      if (this->i2c_.init() != core::ErrorCode::kOk) {
        AppLogger::Error("Nie udało sie zainicjować");
      }
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
