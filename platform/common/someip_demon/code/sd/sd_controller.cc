#include "platform/common/someip_demon/code/sd/sd_controller.h"

#include "ara/com/someip/EndpointOption.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/someip_frame.h"
#include "ara/com/type_converter.h"
#include "ara/log/logging_menager.h"
#include "core/common/condition.h"

namespace simba {
namespace someip_demon {
namespace someip {
namespace sd {

std::vector<ara::com::someip::ServiceEntry> SdController::ParseServiceList(
    const std::vector<uint8_t>& raw) {
  std::vector<ara::com::someip::ServiceEntry> res{};
  auto count = (ara::com::Convert<uint32_t>::Conv(raw).value_or(0U));
  if constexpr (std::endian::native != std::endian::big) {
    count = ara::com::EndianConvert<std::uint32_t>::Conv(count) / 16;
  }
  logger_.LogInfo() << "Service List count: " << count;
  for (auto i = 0; i < count; i++) {
    if (*(raw.begin() + 4 + (16 * i)) == 0x01 ||
        *(raw.begin() + 4 + (16 * i)) == 0x00) {
      const auto t = ara::com::Convert<ara::com::someip::ServiceEntry>::Conv(
          std::vector<uint8_t>{raw.begin() + 4 + (16 * i), raw.end()});
      if (t.HasValue()) {
        res.push_back(t.Value());
      } else {
        logger_.LogError() << "Error in (ParseServiceList): "
                           << t.Error().Message() << " -> "
                           << t.Error().SupportData();
      }
    }
  }
  return std::move(res);
}

void SdController::ProcessFrame(
    const std::string& ip, const std::uint16_t& port,
    const ara::com::someip::SomeipFrame& frame) noexcept {
  const auto list = ParseServiceList(
      std::vector<uint8_t>{frame.Payload().begin() + 4, frame.Payload().end()});
}

ara::core::Result<void> SdController::Start() noexcept {
  if (sd_loop_ != nullptr) {
    return {};
  }
  sd_loop_ = std::make_unique<std::jthread>(
      std::bind(&SdController::SdLoop, this, std::placeholders::_1));
  return {};
}

void SdController::Stop() noexcept {}

void SdController::SetTransmitCallback(
    common::IController::TransmitCallback callback) noexcept {
  this->callback_ = callback;
}

SdController::SdController(
    std::shared_ptr<common::com::IMulticastController> multicast_controller)
    : multicast_controller_{multicast_controller},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger("sd  ")} {
  sd_db_.AddNewProvideService(
      db::ServiceItem{0x1234, 0x5678, 0x01, 0x01, 0xC0A80A65, 0x1111});
}

SdController::~SdController() {}

void SdController::SdLoop(std::stop_token token) {
  logger_.LogInfo() << "Loop Start";
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::seconds{1}, token);
    if (multicast_controller_ != nullptr) {
      for (const auto& item : sd_db_.GetProvideList()) {
        logger_.LogInfo() << "Offer";
        this->OfferService(item.second);
      }
    }
  }
}
void SdController::OfferService(const db::ServiceItem& item) noexcept {
  ara::com::someip::HeaderStructure header{0xFFFF, 0x8100, 0x00, 0x00, 0x01,
                                           0x01,   0x01,   0x02, 0x00};
  const uint8_t flag{0xc0};
  const uint8_t res1{0x00};
  const uint8_t res2{0x00};
  const uint8_t res3{0x00};
  const uint32_t entry_length{16};

  std::vector<uint8_t> payload{flag, res1, res2, res3};

  if constexpr (std::endian::native == std::endian::big) {
    const auto res = ara::com::Convert2Vector<uint32_t>::Conv(entry_length);
    payload.insert(payload.end(), res.begin(), res.end());
  } else {
    const auto temp_v =
        ara::com::EndianConvert<std::uint32_t>::Conv(entry_length);
    const auto res = ara::com::Convert2Vector<uint32_t>::Conv(temp_v);
    payload.insert(payload.end(), res.begin(), res.end());
  }
  const ara::com::someip::ServiceEntry service{0x01,  // TYPE
                                               0x00,  // Index
                                               0x00,  // Index
                                               0x10,  // Opt
                                               item.service_id_,
                                               item.instance_id_,
                                               item.major_version_,
                                               0x05,
                                               item.minor_version_};

  {
    const auto t =
        ara::com::Convert2Vector<ara::com::someip::ServiceEntry>::Conv(service);
    payload.insert(payload.end(), t.begin(), t.end());
  }

  const uint32_t endpoint_length{12};
  if constexpr (std::endian::native == std::endian::big) {
    const auto res = ara::com::Convert2Vector<uint32_t>::Conv(endpoint_length);
    payload.insert(payload.end(), res.begin(), res.end());
  } else {
    const auto temp_v =
        ara::com::EndianConvert<std::uint32_t>::Conv(endpoint_length);
    const auto res = ara::com::Convert2Vector<uint32_t>::Conv(temp_v);
    payload.insert(payload.end(), res.begin(), res.end());
  }
  const ara::com::someip::EndpointOption endpoint{
      0x09, 0x04, 0x00, item.ip_, 0x00, 0x11, item.port_};
  {
    const auto t2 =
        ara::com::Convert2Vector<ara::com::someip::EndpointOption>::Conv(
            endpoint);
    payload.insert(payload.end(), t2.begin(), t2.end());
  }

  const auto frame = ara::com::someip::SomeipFrame::MakeFrame(header, payload);
  //   this->callback_(0, 0, frame);
  logger_.LogInfo() << "Sendding offer";
  multicast_controller_->SendFrame(frame.GetRaw(), "", 0);
}
}  // namespace sd
}  // namespace someip
}  // namespace someip_demon
}  // namespace simba
