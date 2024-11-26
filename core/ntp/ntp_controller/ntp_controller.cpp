/**
 * @file ptp_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/ntp/ntp_controller/ntp_controller.hpp"
#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include <string>
#include <memory>
#include <vector>
#include "ara/core/instance_specifier.h"
#include "ara/log/log.h"
#include "core/json/json_parser.h"
#include "core/ntp/data/header.hpp"
#include "core/common/condition.h"
#include "core/ntp/timestamp/global_timestamp.hpp"

namespace simba {
namespace ntp {

namespace {
  constexpr auto kShm_name = "global_timestamp";
  constexpr auto kNtp_port = 123;
  constexpr auto kNtp_interval_ms = 3000;
}  // namespace



NTPController::NTPController(): shm_(ara::core::InstanceSpecifier{kShm_name}) {
    sock_.SetRXCallback([this](const std::string& ip, const std::uint16_t& port,
                       std::vector<std::uint8_t> payload) {
    if (payload.size() != sizeof(Header)) {
        return;
    }
    auto now = this->GetNowTimestamp();
    Header hdr_(0, 0, 0, 0, 0);
    hdr_.SetBuffor(payload);
    if (hdr_.GetT2() == 0 && hdr_.GetT3() == 0) {
        hdr_.SetT2(now);
        hdr_.SetT3(this->GetNowTimestamp());
        this->sock_.Transmit("192.168.10."+std::to_string(hdr_.GetID()), kNtp_port, hdr_.GetBuffor());
    }
    if (hdr_.GetT3() != 0 && hdr_.GetT4() == 0) {
        hdr_.SetT4(now);
        this->CorrectStartPoint(hdr_.GetT1(), hdr_.GetT2(), hdr_.GetT3(), hdr_.GetT4());
    }
    ara::log::LogWarn() << "now timestamp" << std::to_string(this->GetNowTimestamp());
    return;
    });
}
void NTPController::CorrectStartPoint(const uint64_t& T1, const uint64_t& T2, const uint64_t& T3, const uint64_t& T4) {
    auto cor = ((T2 - T1) + (T3 -T4)) / 2;
    this->start_timestamp += cor;
    ara::log::LogInfo() << "diffrence beetwin master and slave: " << std::to_string(static_cast<int>(cor)) << "ms";
    this->shm_.Send(this->start_timestamp);
    ara::log::LogInfo() << "Round Trip delay " << std::to_string(((T4 - T1) - (T3 - T2))) << "ms";
}

void NTPController::transmit_loop(std::stop_token token) {
    Header hdr_(0, 0, 0, 0, id);
    while (!token.stop_requested()) {
        hdr_.SetT1(GetNowTimestamp());
        sock_.Transmit(this->master_, kNtp_port, hdr_.GetBuffor());
        core::condition::wait_for(std::chrono::seconds(kNtp_interval_ms), token);
    }
}

uint64_t NTPController::GetNowTimestamp() {
    return start_timestamp - GetNowInMS();
}

int NTPController::Init(const std::string& master, const uint8_t& id) {
    this->id = id;
    auto parser = core::json::JsonParser::Parser(kIp_file_path);
    if (!parser.has_value()) {
        return 1;
    }
    const auto ip = parser.value().GetString("ip");
    if (!ip.has_value()) {
        return 1;
    }
    sock_.Init(com::soc::SocketConfig{ip.value(), 123, 10123});
    auto res = shm_.OfferService();
    if (!res.HasValue()) {
        ara::log::LogWarn() << res.Error();
        return 1;
    }
    start_timestamp = GetNowInMS();
    auto res2 = shm_.Send(start_timestamp);
    if (!res2.HasValue()) {
        ara::log::LogWarn() << res2.Error();
        return 1;
    }
    sock_.StartRXThread();
    this->transmit_thread = std::make_unique<std::jthread>(transmit_loop);
    return 0;
}
NTPController::~NTPController() {
    transmit_thread->request_stop();
    sock_.StopRXThread();
    transmit_thread->join();
}

}  // namespace ntp
}  // namespace simba
