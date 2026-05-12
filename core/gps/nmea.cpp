/**
 * @file nmea.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/gps/nmea.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <algorithm>
namespace srp {
namespace core {

namespace {
    constexpr float knotsToKilometersPerHour = 1.852f;
}

std::vector<std::string> Nmea::splitString(const std::string& str, const char& delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}
std::optional<Nmea::NmeaType> Nmea::Parse(const std::string& gps_data) {
    try {
    const auto start_p = gps_data.find(',');
    if (start_p == std::string::npos) {
        return std::nullopt;
    }
    const std::string seq_type = gps_data.substr(0, start_p);
    const std::string data_part = gps_data.substr(start_p + 1);
    auto res = splitString(data_part);

    if (seq_type == "$GNGGA") {
        if (res.size() != 14) {
            return std::nullopt;
        }
        GPS_DATA_T data;
        if (res[5] == "0") {
            return std::nullopt;
        }
        data.timestamp = std::stod(res[0]);
        data.latitude = std::stod(res[1]);
        data.latitude_dir = res[2][0];
        data.longitude = std::stod(res[3]);
        data.longitude_dir = res[4][0];
        data.satellite_nr = std::stoul(res[6]);
        data.HDOP = std::stof(res[7]);
        data.height = std::stof(res[8]);
        return NmeaType{data};
    } else if (seq_type == "$GNRMC") {
        if (res.size() < 8) {
            return std::nullopt;
        }

        GPS_DATA_RMC_T data;
        if (res[1] == "V") {
            return std::nullopt;
        }

        data.timestamp = std::stod(res[0]);
        data.latitude = std::stod(res[2]);
        data.latitude_dir = res[3][0];
        data.longitude = std::stod(res[4]);
        data.longitude_dir = res[5][0];
        data.speed = std::stod(res[6]);
        data.angle = std::stod(res[7]);
        return NmeaType{data};
    } else if (seq_type == "$GNVTG") {
        if (res.size() < 8) {
            return std::nullopt;
        }

        GPS_DATA_VTG_T data;
        if (res.size() > 8 && !res[8].empty() && res[8][0] != 'A' && res[8][0] != 'D') {
            return std::nullopt;
        }

        data.trueCourseOverGround = std::stof(res[0]);
        data.relativeSpeed = std::stof(res[6]);

        data.trueCourseOverGround *= knotsToKilometersPerHour;
        return NmeaType{data};
    }


    return std::nullopt;
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

}  // namespace core
}  // namespace srp
