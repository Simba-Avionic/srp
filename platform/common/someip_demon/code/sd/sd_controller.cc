/**
 * @file sd_controller.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "platform/common/someip_demon/code/sd/sd_controller.h"

#include <utility>

#include "ara/com/someip/EndpointOption.h"
#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/someip_frame.h"
#include "data/type_converter.h"
#include "ara/log/logging_menager.h"
#include "core/common/condition.h"

namespace srp {
namespace someip_demon {
namespace someip {
namespace sd {

std::vector<db::FindServiceItem> SdController::ParseServiceList(
    const std::vector<uint8_t>& raw) {
  std::vector<ara::com::someip::ServiceEntry> res{};
  auto count = (srp::data::Convert<uint32_t>::Conv(raw).value_or(0U));
  if constexpr (std::endian::native != std::endian::big) {
    count = srp::data::EndianConvert<std::uint32_t>::Conv(count) / 16;
  }

  for (auto i = 0; i < count; i++) {
    if (*(raw.begin() + 4 + (16 * i)) == 0x01) {
      const auto t = srp::data::Convert<ara::com::someip::ServiceEntry>::Conv(
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
  std::vector<uint8_t> ip_raw{raw.begin() + 4 + 16 * count, raw.end()};
  std::vector<ara::com::someip::EndpointOption> ip_res{};
  auto ip_count = (srp::data::Convert<uint32_t>::Conv(ip_raw).value_or(0U));
  if constexpr (std::endian::native != std::endian::big) {
    ip_count = srp::data::EndianConvert<std::uint32_t>::Conv(ip_count) / 12;
  }

  for (auto i = 0; i < ip_count; i++) {
    const auto t = srp::data::Convert<ara::com::someip::EndpointOption>::Conv(
        std::vector<uint8_t>{ip_raw.begin() + 4 + (12 * i), ip_raw.end()});
    if (t.has_value()) {
      ip_res.push_back(t.value());
    } else {
      logger_.LogError() << "Error in (EndpointOptionList): ";
    }
  }
  std::vector<db::FindServiceItem> item_list{};
  for (const auto& item : res) {
    db::FindServiceItem new_service{item.service_id, item.instance_id,
                                    item.major_version, item.minor_version};
    new_service.ip_ = ip_res.at(item.index_1).ip;
    new_service.port_ = ip_res.at(item.index_1).port;
    item_list.push_back(std::move(new_service));
  }
  return std::move(item_list);
}

void SdController::ProcessFrame(
    const std::string& ip, const std::uint16_t& port,
    const ara::com::someip::SomeipFrame& frame) noexcept {
  const auto list = ParseServiceList(
      std::vector<uint8_t>{frame.Payload().begin() + 4, frame.Payload().end()});

  bool reboot_db{false};
  for (const auto& item : list) {
    const auto& sd_item =
        this->sd_db_.FindInFindService(item.service_id_, item.instance_id_);
    if (sd_item.has_value()) {
      auto& service_sd = sd_item.value().get();
      if (service_sd.ip_ != item.ip_) {
        service_sd.ip_ = item.ip_;
        service_sd.port_ = item.port_;
        reboot_db = true;
        logger_.LogInfo() << "Service: " << item.service_id_ << ":"
                          << item.instance_id_ << " founded! ("
                          << service_sd.ip_ << ":" << service_sd.port_ << ")";
      }
    }
  }
  if (reboot_db) {
    sd_db_.NotifyAboutNewService();
  }
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
    std::shared_ptr<common::com::IMulticastController> multicast_controller,
    db::Database& sd_db)
    : multicast_controller_{multicast_controller},
      logger_{ara::log::LoggingMenager::GetInstance()->CreateLogger(
          "sd  ", "", ara::log::LogLevel::kInfo)},
      sd_db_{sd_db} {}

SdController::~SdController() {}

void SdController::SdLoop(std::stop_token token) {
  logger_.LogInfo() << "Loop Start";
  while (!token.stop_requested()) {
    core::condition::wait_for(std::chrono::seconds{1}, token);
    if (multicast_controller_ != nullptr) {
      logger_.LogDebug() << "Find ("
                        << static_cast<uint8_t>(sd_db_.GetConsumeList().size())
                        << ")";
      this->FindService();
      logger_.LogDebug() << "Offer ("
                        << static_cast<uint8_t>(sd_db_.GetProvideList().size())
                        << ")";
      this->OfferService();
    }
  }
}

void SdController::OfferService() noexcept {
  ara::com::someip::SomeipSdFrameBuilder builder{};
  for (const auto& item : sd_db_.GetProvideList()) {
    builder.AddOfferEntry(item.second.service_id_, item.second.instance_id_,
                          item.second.major_version_,
                          item.second.minor_version_, item.second.ip_,
                          item.second.port_);
    if (builder.GetSize() > 10) {
      multicast_controller_->SendFrame(builder.BuildFrame().GetRaw(), "", 0);
      builder = ara::com::someip::SomeipSdFrameBuilder{};
    }
  }
  if (builder.GetSize() > 0) {
    multicast_controller_->SendFrame(builder.BuildFrame().GetRaw(), "", 0);
  }
}

void SdController::FindService() noexcept {
  ara::com::someip::SomeipSdFrameBuilder builder{};
  for (const auto& i : sd_db_.GetConsumeList()) {
    if (i.second.ip_ == 0 && i.second.port_ == 0) {
      builder.AddFindEntry(i.second.service_id_, i.second.instance_id_,
                           i.second.major_version_, i.second.minor_version_);
      if (builder.GetSize() > 10) {
        multicast_controller_->SendFrame(builder.BuildFrame().GetRaw(), "", 0);
        builder = ara::com::someip::SomeipSdFrameBuilder{};
      }
    }
  }
  if (builder.GetSize() != 0) {
    multicast_controller_->SendFrame(builder.BuildFrame().GetRaw(), "", 0);
  }
}
}  // namespace sd
}  // namespace someip
}  // namespace someip_demon
}  // namespace srp
