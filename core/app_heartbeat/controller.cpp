/**
 * @file controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-04-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "core/app_heartbeat/controller.hpp"
#include "core/common/condition.h"
#include "core/app_heartbeat/data_structure.h/srp/core/hb/HBHeader.h"
#include "ara/log/log.h"
namespace srp {
namespace core {
namespace hb {

namespace {
    static constexpr auto kRx_sock_name = "HB_SOCK";
}

void HeartBeatController::Loop(const std::stop_token& t) {
    HBHeader hdr{id_};

    auto data = srp::data::Convert2Vector<HBHeader>::Conv(hdr);
    while (!t.stop_requested()) {
        if (sock_.Transmit(kRx_sock_name, 0, data) != core::ErrorCode::kOk) {
            ara::log::LogWarn() << "cant send hb from service: " << std::to_string(id_); 
        } else {
            ara::log::LogDebug() << "send hb from service: " <<  std::to_string(id_); 
        }
        condition::wait_for(std::chrono::seconds(1), t);
    }
}

HeartBeatController::HeartBeatController() {}
void HeartBeatController::Init(const SERVICES_e id) {
    this->id_ = id;
    thread = std::jthread([this](const std::stop_token& token) {
        Loop(token);
    });
}

}  // namespace hb
}  // namespace core
}  // namespace srp
