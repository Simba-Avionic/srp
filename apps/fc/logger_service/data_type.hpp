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

 public:
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
