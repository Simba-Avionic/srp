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

#include <memory>
#include <vector>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include "communication-core/someip-controller/event_proxy.h"
#include "communication-core/someip-controller/method_proxy.h"
#include "communication-core/someip-controller/method_skeleton.h"
#include "core/logger/Logger.h"
#include "diag/dtc/controller/dtc.h"
#include "mw/i2c_service/data/i2c_factory.h"
namespace simba {
namespace router {

namespace {
  const constexpr uint8_t BME280_ADDR = 0x76;
  const constexpr uint8_t BME280_PRESS_REG = 0xF7;
}

core::ErrorCode Router::Run(std::stop_token token) {
  while (true) {
  auto res = this->i2c_.Read(76, BME280_PRESS_REG, 3);
  AppLogger::Warning("Receive response");
  if (!res.has_value()) {
    AppLogger::Warning("NO VALUE IN RES");
  }
  auto hdr = i2c::I2CFactory::GetHeader(res.value());
  auto payload = i2c::I2CFactory::GetPayload(res.value());
  std::string str;
  for (auto i = 0; i < hdr->GetPayloadSize(); i++) {
    str+=std::to_string(static_cast<int>(payload[i]));
    str+=";";
  }
  AppLogger::Warning("RES:"+str);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  } 
  // int i2c_fd = open("/dev/i2c-2", O_RDWR);
  //   if (i2c_fd < 0) {
  //     exit(1);
  //   }
  //   if (ioctl(i2c_fd, I2C_SLAVE, BME280_ADDR) < 0) {
  //       exit(2);
  //   }
  //      // Konfiguracja rejestru kontrolnego (register control) dla czujnika BME280
  //   unsigned char ctrl_reg[2] = {0xF2, 0x05}; // Ustawienie oversamplingu dla pomiaru temperatury (x1)
  //   if (write(i2c_fd, ctrl_reg, 2) != 2) {
  //       exit(9);
  //   }

  //   // Konfiguracja rejestru konfiguracyjnego (configuration) dla czujnika BME280
  //   unsigned char config_reg[2] = {0xF4, 0xB7}; // Ustawienie trybu pracy (normal mode), czasy próbkowania dla temperatury
  //   if (write(i2c_fd, config_reg, 2) != 2) {
  //       exit(8);
  //   }
  //   // Konfiguracja rejestru konfiguracyjnego (configuration) dla czujnika BME280
  //   unsigned char config_reg2[2] = {0xF5, 0}; // Ustawienie trybu pracy (normal mode), czasy próbkowania dla temperatury
  //   if (write(i2c_fd, config_reg2, 2) != 2) {
  //       exit(8);
  //   }
  //   while (true) {
  //   unsigned char buf[3];
  //   if (write(i2c_fd, &BME280_PRESS_REG, 1) != 1) {
  //       exit(3);
  //   }
  //   if (read(i2c_fd, buf, 3) != 3) {
  //       exit(4);
  //   }

  //   uint64_t press_raw = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4);
  //   float pressure = press_raw / 512;
  //   AppLogger::Warning("raw:"+std::to_string(press_raw));
  //   AppLogger::Warning(std::to_string(pressure));
  //   // Odczyt danych temperatury
  //   unsigned char reg[1] = {0xFA}; // Rejestr, w którym przechowywana jest temperatura
  //   if (write(i2c_fd, reg, 1) != 1) {
  //       exit(5);
  //   }

  //   char data[3];
  //   if (read(i2c_fd, data, 2) != 2) {
  //       exit(6);
  //   }

  //   // Konwersja danych na wartość temperatury w stopniach Celsjusza
  //   int temp_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
  //   float temperature = static_cast<float>(temp_raw / 16000.0);

  //   AppLogger::Warning("Temperature: "+std::to_string(temperature)+" °C");

  //   unsigned char re[1] = {0xFD};
  //   if (write(i2c_fd,re,1)!=1) {
  //     exit(11);
  //   }
  //   char data3[2];
  //   if (read(i2c_fd,data3,2)!=2) {
  //     exit(12);
  //   }
  //   int raw_hum = (data[0] << 8) | data[1];
  //   float hum = static_cast<float>(raw_hum/1024.0);
  //   AppLogger::Warning("Humidity"+std::to_string(hum));


  //   std::this_thread::sleep_for(std::chrono::seconds(2));
  //   }

  return core::ErrorCode::kOk;
}

core::ErrorCode Router::Initialize(
    const std::unordered_map<std::string, std::string>& parms) {
      this->i2c_.Init(this->app_id_);
  return core::ErrorCode::kOk;
}
}  // namespace router
}  // namespace simba
