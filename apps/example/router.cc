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
      this->i2c_.init();
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
