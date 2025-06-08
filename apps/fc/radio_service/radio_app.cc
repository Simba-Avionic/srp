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
#include "apps/fc/main_service/rocket_state.h"

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
constexpr auto kSystemId = 1;
constexpr auto kComponentId = 200;
constexpr auto kTime = 1000;
constexpr auto kBufferSize = 12;
}  // namespace

void RadioApp::TransmittingLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  timestamp_->Init();
  event_data = EventData::GetInstance();
  auto send = [&](auto pack_func) {
    pack_func();
    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
    mavl_logger.LogDebug() << std::vector<uint8_t>(buffer, buffer + len);
    uart_->Write(std::vector<uint8_t>(buffer, buffer + len));
  };
  while (!token.stop_requested()) {
    auto start = std::chrono::high_resolution_clock::now();
    {
    const auto temp1 = event_data->GetTemp1();
    const auto temp2 = event_data->GetTemp2();
    const auto temp3 = event_data->GetTemp3();
    const auto Dpress = event_data->GetDPress();
    const auto press = event_data->GetPress();
    const auto gpsLat = event_data->GetGPSLat();
    const auto gpsLon = event_data->GetGPSLon();
    const auto actuator = event_data->GetActuator();
    send([&] { mavlink_msg_simba_tank_temperature_pack(kSystemId, kComponentId, &msg, temp1, temp2, temp3); });
    send([&] { mavlink_msg_simba_tank_pressure_pack(kSystemId, kComponentId, &msg, Dpress, press); });
    // TODO(m.mankowski2004@gmail.com): change altitude
    send([&] { mavlink_msg_simba_gps_pack(kSystemId, kComponentId, &msg, gpsLon, gpsLat, 0); });
    auto val = timestamp_->GetNewTimeStamp();
    if (val.has_value()) {
      // TODO(m.mankowski2004@gmail.com): add later the status of both computers
      send([&] {
        mavlink_msg_simba_heartbeat_pack(
          kSystemId, kComponentId, &msg, static_cast<uint64_t>(val.value()), 0, 0);
      });
    }
    send([&] { mavlink_msg_simba_actuator_pack(kSystemId, kComponentId, &msg, actuator); });
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    core::condition::wait_for(std::chrono::milliseconds(kTime - duration.count()), token);  // 1 Hz
  }
}

void RadioApp::ListeningLoop(const std::stop_token& token) {
  mavlink_message_t msg;
  mavlink_status_t status;

  while (!token.stop_requested()) {
    auto bytes_read_opt = uart_->Read(kBufferSize);
    if (!bytes_read_opt.has_value()) {
      continue;
    }
    auto bytes_read = bytes_read_opt.value();
    for (uint8_t byte : bytes_read) {
      if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &status)) {
        uint8_t status = 0;
        switch (msg.msgid) {
          case 144: {
            uint8_t abort = RocketState_t::ABORD;
            auto result = this->main_service_handler->setMode(abort);
            status = result.HasValue() ? result.Value() : false;
            break;
          }
          case 145: {
            // uint8_t hold = ;
            // auto result = this->main_service_handler->setMode(hold);
            // status = result.HasValue() ? result.Value() : false;
            break;
          }
          case 146: {
            uint8_t cmd_change = mavlink_msg_simba_cmd_change_get_cmd_change(&msg);
            auto result = this->main_service_handler->setMode(cmd_change);
            status = result.HasValue() ? result.Value() : false;
          }
          case 147: {
            uint8_t actuator_id = mavlink_msg_simba_actuator_cmd_get_actuator_id(&msg);
            uint8_t value = mavlink_msg_simba_actuator_cmd_get_value(&msg);
            status = ActuatorCMD(actuator_id, value);
            break;
          }
        }
        SendAck(msg.msgid, msg.seq, status);
      }
    }
  }
}

void RadioApp::SendAck(uint8_t msgId, uint8_t msgSeq, uint8_t status) {
  mavlink_message_t msg;
  uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
  // change to actual ack pack func
  mavlink_msg_simba_gps_pack(kSystemId, kComponentId, &msg, msgId, msgSeq, status);
  uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);
  mavl_logger.LogDebug() << std::vector<uint8_t>(buffer, buffer + len);
  uart_->Write(std::vector<uint8_t>(buffer, buffer + len));
}

bool RadioApp::ActuatorCMD(uint8_t actuator_id, uint8_t value) {
  switch (actuator_id) {
    case 1: {
      auto result = this->servo_service_handler->SetMainServoValue(value);
      return result.HasValue() ? result.Value() : false;
      break;
    }
    case 2: {
      auto result = this->servo_service_handler->SetVentServoValue(value);
      return result.HasValue() ? result.Value() : false;
      break;
    }
    case 3: {
      if (value == 1) {
        auto result = this->recovery_service_handler->OpenReefedParachute();
        return result.HasValue() ? result.Value() : false;
      }
      break;
    }
    case 4: {
      if (value == 1) {
        auto result = this->recovery_service_handler->UnreefeParachute();
        return result.HasValue() ? result.Value() : false;
      }
      break;
    }
  }
  return false;
}
int RadioApp::Run(const std::stop_token& token) {
  std::jthread transmitting_thread([this](const std::stop_token& t) {
    this->TransmittingLoop(t);
    });
  std::jthread listening_thread([this](const std::stop_token& t) {
    this->ListeningLoop(t);
  });
  core::condition::wait(token);
  service_ipc->StopOffer();
  service_udp->StopOffer();
  uart_->Close();
  return core::ErrorCode::kOk;
}

void RadioApp::InitUart(std::unique_ptr<core::uart::IUartDriver> uart) {
    this->uart_ = std::move(uart);
}

void RadioApp::InitTimestamp(std::unique_ptr<core::timestamp::ITimestampController> timestamp) {
    this->timestamp_ = std::move(timestamp);
}

int RadioApp::Initialize(const std::map<ara::core::StringView,
   ara::core::StringView> parms) {
    if (!this->uart_) {
        auto uart_d = std::make_unique<core::uart::UartDriver>();
        InitUart(std::move(uart_d));
    }
    if (!this->uart_->Open(KGPS_UART_path, KGPS_UART_baudrate)) {
        return 1;
      }
    if (!this->timestamp_) {
        auto timestamp_d = std::make_unique<core::timestamp::TimestampController>();
        InitTimestamp(std::move(timestamp_d));
    }
    service_ipc = std::make_unique<apps::RadioServiceSkeleton>(service_ipc_instance);
    service_udp = std::make_unique<apps::RadioServiceSkeleton>(service_udp_instance);
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
          event_data->SetTemp1(res.Value());
        });
      });
      env_service_handler->newTempEvent_2.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_2.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_2.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetTemp2(res.Value());
        });
      });
      env_service_handler->newTempEvent_3.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newTempEvent_3.SetReceiveHandler([this] () {
          auto res = env_service_handler->newTempEvent_3.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetTemp3(res.Value());
        });
      });
      env_service_handler->newDPressEvent.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newDPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newDPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetDPress(res.Value());
        });
      });
      env_service_handler->newPressEvent.Subscribe(1, [this](const uint8_t status) {
        env_service_handler->newPressEvent.SetReceiveHandler([this] () {
          auto res = env_service_handler->newPressEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          event_data->SetPress(res.Value());
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
          event_data->SetGPS(res.Value().longitude, res.Value().latitude);
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
          uint8_t bit_position = 0;
          event_data->SetActuatorBit(res.Value(), bit_position);
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
          uint8_t bit_position = 1;
          event_data->SetActuatorBit(res.Value(), bit_position);
        });
      });
      servo_service_handler->ServoVentStatusEvent.Subscribe(1, [this](const uint8_t status) {
        servo_service_handler->ServoVentStatusEvent.SetReceiveHandler([this] () {
          auto res = servo_service_handler->ServoVentStatusEvent.GetNewSamples();
          if (!res.HasValue()) {
            return;
          }
          uint8_t bit_position = 2;
          event_data->SetActuatorBit(res.Value(), bit_position);
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
servo_service_handler{nullptr},
main_service_handler{nullptr},
recovery_service_handler{nullptr},
mavl_logger{ara::log::LoggingMenager::GetInstance()->CreateLogger("MAVL", "", ara::log::LogLevel::kDebug)}
{
}
}  // namespace apps
}  // namespace srp
