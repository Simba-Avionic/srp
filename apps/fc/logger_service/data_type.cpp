/**
 * @file data_type.cpp
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "apps/fc/logger_service/data_type.hpp"

#include <iomanip>
#include <sstream>

namespace srp {
namespace logger {

namespace {
  static constexpr auto kCsv_separator = ";";
  static constexpr auto kCsvHeader =
      "TIMESTAMP;BOARD_TEMP1;BOARD_TEMP2;BOARD_TEMP3;BME_TEMP;BME_HUMIDITY;"
      "BME_ALTITUDE;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION;APOGEE_DETECTED;MAIN_PARACHUTE_DETECTED;"
      "FC_MODE;GPS_LAT;GPS_LON;GPS_ALT;GYRO_X;GYRO_Y;GYRO_Z;ACCEL_X;ACCEL_Y;ACCEL_Z";
}

std::string Data_t::get_header() {
  return kCsvHeader;
}

std::string Data_t::to_string(const std::string& timestamp) {
  const auto t1 = board_temp1_.load(std::memory_order_relaxed);
  const auto t2 = board_temp2_.load(std::memory_order_relaxed);
  const auto t3 = board_temp3_.load(std::memory_order_relaxed);

  const auto bme_temp = bme_temp_.load(std::memory_order_relaxed);
  const auto bme_hum  = bme_humidity_.load(std::memory_order_relaxed);
  const auto bme_alt  = bme_altitude_.load(std::memory_order_relaxed);

  const auto cpu  = sys_cpu_usage_.load(std::memory_order_relaxed);
  const auto mem  = sys_mem_usage_.load(std::memory_order_relaxed);
  const auto disk = sys_disk_utilization_.load(std::memory_order_relaxed);
  const auto apogee  = static_cast<int>(apogee_detected_.load(std::memory_order_relaxed));
  const auto mainParachute = static_cast<int>(main_parachute_detected_.load(std::memory_order_relaxed));
  const auto fc_mode = static_cast<int>(FC_mode.load(std::memory_order_relaxed));

  const auto lat = lat_.load(std::memory_order_relaxed);
  const auto lon = lon_.load(std::memory_order_relaxed);
  const auto alt = alt_.load(std::memory_order_relaxed);

  const auto gyrox = gyro_x_.load(std::memory_order_relaxed);
  const auto gyroy = gyro_y_.load(std::memory_order_relaxed);
  const auto gyroz = gyro_z_.load(std::memory_order_relaxed);

  const auto accelx = accel_x_.load(std::memory_order_relaxed);
  const auto accely = accel_y_.load(std::memory_order_relaxed);
  const auto accelz = accel_z_.load(std::memory_order_relaxed);

  std::stringstream res;
  res << std::fixed << std::setprecision(2);
  res << timestamp << kCsv_separator;
  res << t1 << kCsv_separator;
  res << t2 << kCsv_separator;
  res << t3 << kCsv_separator;
  res << bme_temp << kCsv_separator;
  res << bme_hum << kCsv_separator;
  res << bme_alt << kCsv_separator;
  res << cpu << kCsv_separator;
  res << mem << kCsv_separator;
  res << disk << kCsv_separator;
  res << apogee << kCsv_separator;
  res << mainParachute << kCsv_separator;
  res << fc_mode << kCsv_separator;
  res << lat << kCsv_separator;
  res << lon << kCsv_separator;
  res << alt << kCsv_separator;
  res << gyrox << kCsv_separator;
  res << gyroy << kCsv_separator;
  res << gyroz << kCsv_separator;
  res << accelx << kCsv_separator;
  res << accely << kCsv_separator;
  res << accelz;
  return res.str();
}

void Data_t::SetIMU(float ax, float ay, float az, float gx, float gy, float gz) {
  accel_x_.store(ax, std::memory_order_relaxed);
  accel_y_.store(ay, std::memory_order_relaxed);
  accel_z_.store(az, std::memory_order_relaxed);
  gyro_x_.store(gx, std::memory_order_relaxed);
  gyro_y_.store(gy, std::memory_order_relaxed);
  gyro_z_.store(gz, std::memory_order_relaxed);
}

void Data_t::SetGpsData(const float lat, const float lot, const float alt) {
  lat_.store(lat, std::memory_order_relaxed);
  lon_.store(lot, std::memory_order_relaxed);
  alt_.store(alt, std::memory_order_relaxed);
}

void Data_t::SetFCMode(const uint8_t mode) {
  FC_mode.store(mode, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp1(tempType temp) {
  board_temp1_.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp2(tempType temp) {
  board_temp2_.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBoardTemp3(tempType temp) {
  board_temp3_.store(temp, std::memory_order_relaxed);
}

void Data_t::SetBmeData(const float temp, const float humidity, const float altitude) {
  bme_temp_.store(temp, std::memory_order_relaxed);
  bme_humidity_.store(humidity, std::memory_order_relaxed);
  bme_altitude_.store(altitude, std::memory_order_relaxed);
}
void Data_t::SetSystemUsage(const float cpu_usage, const float mem_usage, const float disk_usage) {
  sys_cpu_usage_.store(cpu_usage, std::memory_order_relaxed);
  sys_mem_usage_.store(mem_usage, std::memory_order_relaxed);
  sys_disk_utilization_.store(disk_usage, std::memory_order_relaxed);
}

void Data_t::SetApogeeDetected(const bool apogee_detected) {
  apogee_detected_.store(apogee_detected, std::memory_order_relaxed);
}

void Data_t::SetMainParachuteDetected(const bool main_parachute_detected) {
  main_parachute_detected_.store(main_parachute_detected, std::memory_order_relaxed);
}
}  // namespace logger
}  // namespace srp
