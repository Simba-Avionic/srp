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
  constexpr auto kService_ipc_instance = "srp/apps/GPSApp/GPSService_ipc";
  constexpr auto kService_udp_instance = "srp/apps/GPSApp/GPSService_udp";
  constexpr auto KGPS_UART_path = "/dev/ttyS1";
  constexpr auto KGPS_UART_baudrate = B230400;
}

GPSDataStructure GPSApp::GetSomeIPData(const core::GPS_DATA_T& data) {
    GPSDataStructure someip_data;
    someip_data.lattitude = data.latitude;
    someip_data.longitude = data.longitude;
    if (data.latitude_dir == 'S') {
      someip_data.lattitude *= -1;
    }
    if (data.longitude_dir == 'W') {
      someip_data.longitude *= -1;
    }
    return someip_data;
}
void GPSApp::Init(std::unique_ptr<core::uart::IUartDriver> uart) {
  this->uart_ = std::move(uart);
}
std::optional<GPSDataStructure> GPSApp::ParseGPSData(const std::vector<char>& data) {
  std::string s(data.begin(), data.end());
  auto res = core::Nmea::Parse(s);
  if (!res.has_value()) {
    return std::nullopt;
  }
  auto someip_data = GetSomeIPData(res.value());
  // TODO(matikrajek42@gmail.com)  uncoment afer basn advice
  // ara::log::LogInfo() << "GPS lattitude: " << someip_data.lattitude
  //     << ", longtitude: " << someip_data.longitude << ",height(M):"
  //     << res.value().height << "satelite_nr: " << res.value().satellite_nr;
  return someip_data;
}

int GPSApp::Run(const std::stop_token& token) {
  while (!token.stop_requested()) {
    auto data = uart_->Read();
    if (!data.has_value()) {
      core::condition::wait_for(std::chrono::milliseconds(10), token);
      continue;
    }
    auto res = ParseGPSData(data.value());
    if (res.has_value()) {
      service_ipc->GPSStatusEvent.Update(res.value());
      service_udp->GPSStatusEvent.Update(res.value());
    }
  }
  core::condition::wait(token);
  service_ipc->StopOffer();
  service_udp->StopOffer();
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
  service_ipc = std::make_unique<apps::GPSServiceSkeleton>(service_ipc_instance);
  service_udp = std::make_unique<apps::GPSServiceSkeleton>(service_udp_instance);
  service_ipc->StartOffer();
  service_udp->StartOffer();
  ara::log::LogInfo() << "End initialization";
  return 0;
}

GPSApp::~GPSApp() {
}

GPSApp::GPSApp(): service_ipc_instance(kService_ipc_instance),
                  service_udp_instance(kService_udp_instance) {
}

}  // namespace apps
}  // namespace srp

