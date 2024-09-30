#ifndef PLATFORM_COMMON_SOMEIP_DEMON_CODE_SD_SD_CONTROLLER_H_
#define PLATFORM_COMMON_SOMEIP_DEMON_CODE_SD_SD_CONTROLLER_H_

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "ara/com/someip/ServiceEntry.h"
#include "ara/log/logger.h"
#include "platform/common/someip_demon/code/common/com/i_multicast_controller.h"
#include "platform/common/someip_demon/code/common/i_controller.h"
#include "platform/common/someip_demon/code/db/database.h"

namespace simba {
namespace someip_demon {
namespace someip {
namespace sd {
class SdController : public common::IController {
 private:
  std::shared_ptr<common::com::IMulticastController> multicast_controller_;
  db::Database sd_db_{};
  common::IController::TransmitCallback callback_;
  std::unique_ptr<std::jthread> sd_loop_;
  const ara::log::Logger& logger_;
  void SdLoop(std::stop_token token);
  void OfferService(const db::ServiceItem& item) noexcept;
  std::vector<ara::com::someip::ServiceEntry> ParseServiceList(
      const std::vector<uint8_t>& raw);

 public:
  void SetTransmitCallback(
      common::IController::TransmitCallback callback) noexcept override;
  ara::core::Result<void> Start() noexcept override;
  void Stop() noexcept override;
  void ProcessFrame(
      const std::string& ip, const std::uint16_t& port,
      const ara::com::someip::SomeipFrame& frame) noexcept override;
  SdController(
      std::shared_ptr<common::com::IMulticastController> multicast_controller);
  ~SdController();
};
}  // namespace sd
}  // namespace someip
}  // namespace someip_demon
}  // namespace simba

#endif  // PLATFORM_COMMON_SOMEIP_DEMON_CODE_SD_SD_CONTROLLER_H_
