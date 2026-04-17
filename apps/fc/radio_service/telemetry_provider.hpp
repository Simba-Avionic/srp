#ifndef APPS_FC_RADIO_SERVICE_TELEMETRY_PROVIDER_HPP_
#define APPS_FC_RADIO_SERVICE_TELEMETRY_PROVIDER_HPP_

#include "simba/mavlink.h"
#include "simba/simba.h"
#include "apps/fc/radio_service/event_data.h"
#include "core/timestamp/timestamp_driver.hpp"
#include "apps/ec/primer_service/controller/primer_controller.hpp"
#include <memory>
#include <vector>
#include <optional>

namespace srp {
namespace apps {
namespace radio {

class TelemetryProvider {
 public:
  TelemetryProvider();

  template <typename PackFunc>
  std::vector<uint8_t> Pack(PackFunc&& pack_func) {
    mavlink_message_t msg;
    pack_func(&msg);

    std::vector<uint8_t> buffer(MAVLINK_MAX_PACKET_LEN);
    uint16_t len = mavlink_msg_to_send_buffer(buffer.data(), &msg);
    
    buffer.resize(len);
    return buffer;
  }

  std::optional<std::vector<uint8_t>> GetHeartbeatMsg();
  std::vector<uint8_t> GetMaxAltitudeMsg();
  std::vector<uint8_t> GetTankSensorsMsg();
  std::vector<uint8_t> GetGpsMsg();
  std::vector<uint8_t> GetComputersTelemetryMsg();

 private:
  std::shared_ptr<EventData> event_data;
  core::timestamp::TimestampController timestamp_;

  uint8_t MapState(core::rocketState::RocketState_t s);
  uint8_t MapPrimer(srp::primer::PrimerState_t p);
};

}  // namespace radio
}  // namespace apps
}  // namespace srp

#endif // APPS_FC_RADIO_SERVICE_TELEMETRY_PROVIDER_HPP_