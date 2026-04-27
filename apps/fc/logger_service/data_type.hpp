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

#include <cstdint>
#include <mutex>  // NOLINT
#include <shared_mutex>
#include <string>

namespace srp {
namespace logger {

class Data_t {
 private:
  using tempType = int16_t;
  using bmeType = float;
  using systemStatType = float;

  std::shared_mutex mutex_;
  tempType board_temp1_{0};
  tempType board_temp2_{0};
  tempType board_temp3_{0};
  bmeType bme_temp_{0.0F};
  bmeType bme_humidity_{0.0F};
  bmeType bme_altitude_{0.0F};
  systemStatType cpu_usage_{0.0F};
  systemStatType mem_usage_{0.0F};
  systemStatType disk_utilization_{0.0F};

 public:
  std::string get_header();
  std::string to_string(const std::string& timestamp);
  void SetBoardTemp1(const tempType& temp);
  void SetBoardTemp2(const tempType& temp);
  void SetBoardTemp3(const tempType& temp);
  void SetBmeTemp(const bmeType& value);
  void SetBmeHumidity(const bmeType& value);
  void SetBmeAltitude(const bmeType& value);
  void SetCpuUsage(const systemStatType& value);
  void SetMemUsage(const systemStatType& value);
  void SetDiskUtilization(const systemStatType& value);
};

}  // namespace logger
}  // namespace srp

#endif  // APPS_FC_LOGGER_SERVICE_DATA_TYPE_HPP_
