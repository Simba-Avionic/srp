#include "platform/common/someip_demon/code/common/network_controller.h"

#include <future>

#include "ara/com/someip/someip_frame.h"
#include "ara/log/logging_menager.h"
namespace simba {
namespace someip_demon {
namespace common {

NetworkController::NetworkController(/* args */)
    : logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("netc")} {}

NetworkController::~NetworkController() {}
void NetworkController::SetController(
    std::shared_ptr<IController> controller,
    INetworkController::ControllerType type) noexcept {
  if (type == INetworkController::ControllerType::kSomeipSd) {
    sd_controller_ = controller;
  }
}

void NetworkController::ProcessRawFrame(
    const std::string& ip, const std::uint16_t& port,
    const std::vector<std::uint8_t>& data) noexcept {
  const auto frame_r = ara::com::someip::SomeipFrame::MakeFrame(data);
  if (!frame_r.HasValue()) {
    logger_.LogError() << "Parsing error for msg from (" << ip << ":" << port
                       << "): " << frame_r.Error().Message();
    return;
  }
  const auto& frame = frame_r.Value();
  logger_.LogInfo() << "New msg from: " << ip << ":" << port
                    << " Service_id: " << frame.header_.service_id
                    << ", method_id: " << frame.header_.method_id;
  if ((frame.header_.service_id == 0xffff) and
      (frame.header_.method_id == 0x8100)) {
    if (sd_controller_ != nullptr) {
      std::async(std::launch::async, [this, &ip, &port, &frame]() {
        sd_controller_->ProcessFrame(ip, port, frame);
      });

    } else {
      logger_.LogError() << "Missing SD controller";
    }
  }
}
}  // namespace common
}  // namespace someip_demon
}  // namespace simba
