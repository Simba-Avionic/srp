/**
 * @file nmea.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_GPS_NMEA_HPP_
#define CORE_GPS_NMEA_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <optional>
namespace simba {
namespace core {

struct GPS_DATA_T {
  double timestamp;
  double latitude;
  char latitude_dir;
  double longitude;
  char longitude_dir;
  uint16_t satellite_nr;
  float HDOP;
  float height;
  std::string to_string() {
    std::ostringstream oss;
    oss << "Timestamp: " << timestamp << ", "
        << "Latitude: " << latitude << " " << latitude_dir << ", "
        << "Longitude: " << longitude << " " << longitude_dir << ", "
        << "Satellites: " << satellite_nr << ", "
        << "HDOP: " << HDOP << ", "
        << "Height: " << height;
    return oss.str();
  }
};

class Nmea {
 public:
  static std::vector<std::string> splitString(const std::string& str, const char& delimiter = ',');
  static std::optional<GPS_DATA_T> Parse(const std::string& gps_data);
};

}  // namespace core
}  // namespace simba

#endif  // CORE_GPS_NMEA_HPP_
