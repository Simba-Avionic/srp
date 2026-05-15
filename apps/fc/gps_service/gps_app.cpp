/**
 * @file gps_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <utility>
#include "apps/fc/gps_service/gps_app.hpp"
#include "ara/exec/adaptive_application.h"
#include "core/common/condition.h"
#include "ara/log/log.h"
namespace srp {
namespace apps {
namespace {
  static constexpr auto kService_ipc_instance =       "srp/apps/GPSApp/GPSService_ipc";
  static constexpr auto kService_udp_instance =       "srp/apps/GPSApp/GPSService_udp";
  static constexpr auto KGPS_UART_path =              "/dev/ttyS4";
  static constexpr auto KGPS_UART_baudrate =          B230400;
  static constexpr uint16_t KGps_expected_interval =  1000;
  static constexpr auto kGps_freq_tolerance =         100;
  static constexpr auto kHeartBeatPinID = 3;
}

GPSDataStructure GPSApp::GetSomeIPData(const core::GPS_DATA_T& data) {
    GPSDataStructure someip_data;
    someip_data.latitude = data.latitude;
    someip_data.longitude = data.longitude;
    someip_data.altitude = data.height;
    if (data.latitude_dir == 'S') {
      someip_data.latitude *= -1;
    }
    if (data.longitude_dir == 'W') {
      someip_data.longitude *= -1;
    }
    return someip_data;
}

GPSRMCDataStructure GPSApp::GetSomeIPData(const core::GPS_DATA_RMC_T& data) {
    GPSRMCDataStructure someip_data;
    someip_data.latitude = data.latitude;
    someip_data.longitude = data.longitude;
    someip_data.speed = data.speed;
    someip_data.angle = data.angle;
    if (data.latitude_dir == 'S') {
      someip_data.latitude *= -1;
    }
    if (data.longitude_dir == 'W') {
      someip_data.longitude *= -1;
    }
    return someip_data;
}

GPSVTGDataStructure GPSApp::GetSomeIPData(const core::GPS_DATA_VTG_T& data) {
    GPSVTGDataStructure someip_data;
    someip_data.trueCourse = data.trueCourseOverGround;
    someip_data.relativeSpeed = data.relativeSpeed;
    return someip_data;
}

void GPSApp::Init(std::unique_ptr<core::uart::IUartDriver> uart) {
  this->uart_ = std::move(uart);
}

std::optional<GPSDataVariant> GPSApp::ParseGPSData(const std::vector<uint8_t>& data) {
  std::string s(data.begin(), data.end());
  auto res = core::Nmea::Parse(s);
  if (!res.has_value()) {
    return std::nullopt;
  }

  const auto* gps_data = std::get_if<core::GPS_DATA_T>(&res.value());
  if (gps_data != nullptr) {
    auto someip_data = GetSomeIPData(*gps_data);
    ara::log::LogDebug() << "GPS latitude: " << someip_data.latitude
        << ", longtitude: " << someip_data.longitude << ",height(M):"
        << gps_data->height << "satelite_nr: " << gps_data->satellite_nr;
    return someip_data;
  }

  const auto* gps_rmc_data = std::get_if<core::GPS_DATA_RMC_T>(&res.value());
  if (gps_rmc_data != nullptr) {
    return GetSomeIPData(*gps_rmc_data);
  }

  const auto* gps_vtg_data = std::get_if<core::GPS_DATA_VTG_T>(&res.value());
  if (gps_vtg_data != nullptr) {
    return GetSomeIPData(*gps_vtg_data);
  }

  return std::nullopt;
}

int64_t GPSApp::GetTimeDelata(const timepoint last_frame_time) const {
  auto now = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_time).count();
}

int GPSApp::Run(const std::stop_token& token) {
  std::jthread hb_thread([this](const std::stop_token& t) {
    while (!t.stop_requested()) {
      if (gpio_.SetPinValue(kHeartBeatPinID, 1, 500) != core::ErrorCode::kOk) {
        ara::log::LogWarn() << "GPSAPP::Run: Failed to toggle heartbeat pin";
      }
      core::condition::wait_for(std::chrono::milliseconds(1000), t);
    }
  });
  std::jthread gps_guard([this](const std::stop_token& t) {
    while (!t.stop_requested()) {
      core::condition::wait_for(std::chrono::milliseconds(100), t);
      {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!first_frame_detected) {
          continue;
        }
        if (GetTimeDelata(last_frame) > KGps_expected_interval + kGps_freq_tolerance) {
            ara::log::LogWarn() << "Missing GPS frame in row: " << warn_num;
            warn_num +=1;
        }
      }
    }
  });
  while (!token.stop_requested()) {
    auto data = uart_->Read();
    if (!data.has_value()) {
      continue;
    }
    auto res = ParseGPSData(data.value());
    if (res.has_value()) {
      if (const auto* gps_data = std::get_if<GPSDataStructure>(&res.value())) {
        service_ipc.GPSStatusEvent.Update(*gps_data);
        service_udp.GPSStatusEvent.Update(*gps_data);
      } else if (const auto* rmc_data = std::get_if<GPSRMCDataStructure>(&res.value())) {
        service_ipc.GPSRMCStatusEvent.Update(*rmc_data);
        service_udp.GPSRMCStatusEvent.Update(*rmc_data);
        continue;
      } else if (const auto* vtg_data = std::get_if<GPSVTGDataStructure>(&res.value())) {
        service_ipc.GPSVTGStatusEvent.Update(*vtg_data);
        service_udp.GPSVTGStatusEvent.Update(*vtg_data);
        continue;
      }

      int64_t delta;
      {
        std::lock_guard<std::mutex> lock(mtx_);
        if (!first_frame_detected) {
          first_frame_detected = true;
        }

        delta = GetTimeDelata(last_frame);

        last_frame = std::chrono::high_resolution_clock::now();
        warn_num = 0;
      }

      if (std::abs(delta - KGps_expected_interval) > kGps_freq_tolerance) {
        ara::log::LogWarn() << "GPS frequency deviation detected: interval = " << delta << " ms";
      }
      ara::log::LogDebug() << "GPS frequency interval = " << delta << " ms";
    }
  }
  service_ipc.StopOffer();
  service_udp.StopOffer();
  uart_->Close();
  return 0;
}

int GPSApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
  if (!uart_) {
    auto uart_d = std::make_unique<core::uart::UartDriver>();
    Init(std::move(uart_d));
  }
  if (!this->uart_->Open(KGPS_UART_path, KGPS_UART_baudrate)) {
    return 1;
  }
  service_ipc.StartOffer();
  service_udp.StartOffer();
  ara::log::LogDebug() << "End initialization";
  last_frame = std::chrono::high_resolution_clock::now();
  return 0;
}

GPSApp::~GPSApp() {
}

GPSApp::GPSApp(): service_ipc{ara::core::InstanceSpecifier{kService_ipc_instance}},
                  service_udp{ara::core::InstanceSpecifier{kService_udp_instance}} {
}

}  // namespace apps
}  // namespace srp
