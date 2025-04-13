/**
 * @file radio_app.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <utility>
#include <vector>
#include "apps/fc/radio_service/radio_app.h"
#include "core/common/condition.h"

namespace srp {
namespace apps {
namespace {
constexpr auto kService_ipc_instance = "srp/apps/RadioApp/RadioService_ipc";
constexpr auto kService_udp_instance = "srp/apps/RadioApp/RadioService_udp";
constexpr auto kEnv_service_path_name = "srp/apps/RadioApp/EnvApp";
constexpr auto kGPS_service_path_name = "srp/apps/RadioApp/GPSService";
constexpr auto kPrimer_service_path_name = "srp/apps/RadioApp/PrimerService";
constexpr auto kServo_service_path_name = "srp/apps/RadioApp/ServoService";
constexpr auto kRecovery_service_path_name = "srp/apps/RadioApp/RecoveryService";
constexpr auto KGPS_UART_path = "/dev/ttyS1";
constexpr auto KGPS_UART_baudrate = B115200;
constexpr auto systemId = 1;
}  // namespace



void RadioApp::TransmittingLoop(const std::stop_token& token) {
  core::timestamp::TimestampController timestamp_;
  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  timestamp_.Init();
  while (!token.stop_requested()) {
    // TODO(m.mankowski2004@gmail.com): Change the component IDs
    std::unique_lock<std::mutex> lock(this->mutex_);
    mavlink_msg_simba_tank_temperature_pack(systemId, 200, &msg,
      temp.temp1, temp.temp2, temp.temp3);
    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));

    mavlink_msg_simba_tank_pressure_pack(systemId, 200, &msg,
      press.Dpressure, press.pressure);
    len = mavlink_msg_to_send_buffer(buffer, &msg);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));

    mavlink_msg_simba_gps_pack(systemId, 200, &msg,
      gps.lon, gps.lat, 0);  // add altitude later
    len = mavlink_msg_to_send_buffer(buffer, &msg);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));

    auto val = timestamp_.GetNewTimeStamp();
    if (val.has_value()) {
      mavlink_msg_simba_heartbeat_pack(systemId, 200, &msg,
        static_cast<uint64_t>(val.value()), 0, 0);  // add later the status of both computers
      len = mavlink_msg_to_send_buffer(buffer, &msg);
      uart_->Write(std::vector<uint8_t>(buffer, buffer + len));
    }
    mavlink_msg_simba_actuator_pack(systemId, 200, &msg,
      actuator.values);
    len = mavlink_msg_to_send_buffer(buffer, &msg);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));
    lock.unlock();
    core::condition::wait_for(std::chrono::milliseconds(1000), token);  // 1 Hz
  }
}

int RadioApp::Run(const std::stop_token& token) {
  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
    });
    core::condition::wait(token);
    service_ipc->StopOffer();
    service_udp->StopOffer();
    uart_->Close();
    return core::ErrorCode::kOk;
}

void RadioApp::Init(std::unique_ptr<core::uart::IUartDriver> uart) {
    this->uart_ = std::move(uart);
}
int RadioApp::Initialize(const std::map<ara::core::StringView,
   ara::core::StringView> parms) {
    if (!this->uart_) {
        auto uart_d = std::make_unique<core::uart::UartDriver>();
        Init(std::move(uart_d));
    }
    if (!this->uart_->Open(KGPS_UART_path, KGPS_UART_baudrate)) {
        return 1;
      }
    this->service_ipc = std::make_unique<apps::RadioServiceSkeleton>
      (this->service_ipc_instance);
    this->service_udp = std::make_unique<apps::RadioServiceSkeleton>
      (this->service_udp_instance);
    service_ipc->StartOffer();
    service_udp->StartOffer();
    this->SomeIpInit();
    return core::ErrorCode::kOk;
}
void RadioApp::SomeIpInit() {
    this->env_service_proxy.StartFindService([this](auto handler) {
      this->env_service_handler = handler;
      env_service_handler->newTempEvent_1.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_1.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_1.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          this->temp.temp1 = res.Value();
        });
      });
      env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          this->temp.temp2 = res.Value();
        });
      });
      env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          this->temp.temp3 = res.Value();
        });
      });
      env_service_handler->newDPressEvent.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newDPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newDPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          this->press.Dpressure = res.Value();
        });
      });
      env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          this->press.pressure = res.Value();
        });
      });
    });
    this->gps_service_proxy.StartFindService([this](auto handler) {
      this->gps_service_handler = handler;
      gps_service_handler->GPSStatusEvent.Subscribe(1, [this](const uint8_t status) {
        gps_service_handler->GPSStatusEvent.SetReceiveHandler([this] () {
          auto res = gps_service_handler->GPSStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          std::memcpy(&this->gps.lon, &res.Value().longitude, sizeof(float));
          std::memcpy(&this->gps.lat, &res.Value().latitude, sizeof(float));
        });
      });
    });
    this->primer_service_proxy.StartFindService([this](auto handler) {
      this->primer_service_handler = handler;
      primer_service_handler->primeStatusEvent.Subscribe(1, [this](const uint8_t status) {
        primer_service_handler->primeStatusEvent.SetReceiveHandler([this] () {
          auto res = primer_service_handler->primeStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          uint8_t bit_position = 0;
          this->actuator.values = (this->actuator.values & ~(1 << bit_position)) | (res.Value() << bit_position);
        });
      });
    });
    this->servo_service_proxy.StartFindService([this](auto handler) {
      this->servo_service_handler = handler;
      servo_service_handler->ServoStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          uint8_t bit_position = 1;
          this->actuator.values = (this->actuator.values & ~(1 << bit_position)) | (res.Value() << bit_position);
        });
      });
      servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          std::unique_lock<std::mutex> lock(this->mutex_);
          uint8_t bit_position = 2;
          this->actuator.values = (this->actuator.values & ~(1 << bit_position)) | (res.Value() << bit_position);
        });
      });
    });
    // TODO(m.mankowski2004@gmail.com): Finish actuators
  }
RadioApp::~RadioApp() {
}
RadioApp::RadioApp(): service_ipc_instance(kService_ipc_instance),
                        service_udp_instance(kService_udp_instance),
env_service_proxy{ara::core::InstanceSpecifier{kEnv_service_path_name}},
env_service_handler{nullptr},
gps_service_proxy{ara::core::InstanceSpecifier{kGPS_service_path_name}},
gps_service_handler{nullptr},
primer_service_proxy{ara::core::InstanceSpecifier{kPrimer_service_path_name}},
primer_service_handler{nullptr},
servo_service_proxy{ara::core::InstanceSpecifier{kServo_service_path_name}},
servo_service_handler{nullptr} {
}

}  // namespace apps
}  // namespace srp
