/**
 * @file data_type.hpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef APPS_FC_LOGGER_SERVICE_DATA_TYPE_HPP_
#define APPS_FC_LOGGER_SERVICE_DATA_TYPE_HPP_

#include <atomic>
#include <cstdint>
#include <string>

namespace srp {
namespace logger {

class Data_t {
 private:
  using tempType = int16_t;

  std::atomic<tempType> board_temp1_{0};
  std::atomic<tempType> board_temp2_{0};
  std::atomic<tempType> board_temp3_{0};

  std::atomic<float> bme_temp_{0.0f};
  std::atomic<float> bme_humidity_{0.0f};
  std::atomic<float> bme_altitude_{0.0f};

  std::atomic<float> sys_cpu_usage_{0.0f};
  std::atomic<float> sys_mem_usage_{0.0f};
  std::atomic<float> sys_disk_utilization_{0.0f};

  std::atomic<bool> apogee_detected_{false};
  std::atomic<bool> main_parachute_detected_{false};

  // IMU
  std::atomic<float> gyro_x_{0.0f};
  std::atomic<float> gyro_y_{0.0f};
  std::atomic<float> gyro_z_{0.0f};

  std::atomic<float> accel_x_{0.0f};
  std::atomic<float> accel_y_{0.0f};
  std::atomic<float> accel_z_{0.0f};

  std::atomic<float> lat_{0.0f};
  std::atomic<float> lon_{0.0f};
  std::atomic<float> alt_{0.0f};

  std::atomic<std::uint16_t> radio_rxerrors_{0};
  std::atomic<std::uint16_t> radio_fixed_{0};
  std::atomic<std::uint8_t> radio_rssi_{0};
  std::atomic<std::uint8_t> radio_remrssi_{0};
  std::atomic<std::uint8_t> radio_txbuf_{0};
  std::atomic<std::uint8_t> radio_noise_{0};
  std::atomic<std::uint8_t> radio_remnoise_{0};

  std::atomic<uint8_t> FC_mode{0};

 public:
  void SetRadioStatus(uint16_t rxerrors, uint16_t fixed, uint8_t rssi,
                  uint8_t remrssi, uint8_t txbuf, uint8_t noise, uint8_t remnoise);
  void SetIMU(float ax, float ay, float az, float gx, float gy, float gz);
  void SetGpsData(const float lat, const float lot, const float alt);
  void SetFCMode(const uint8_t mode);
  std::string get_header();
  std::string to_string(const std::string& timestamp);
  void SetBoardTemp1(tempType temp);
  void SetBoardTemp2(tempType temp);
  void SetBoardTemp3(tempType temp);
  void SetBmeData(const float temp, const float humidity, const float altitude);
  void SetSystemUsage(const float cpu_usage, const float mem_usage, const float disk_usage);
  void SetApogeeDetected(const bool apogee_detected);
  void SetMainParachuteDetected(const bool main_parachute_detected);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_DATA_TYPE_HPP_
