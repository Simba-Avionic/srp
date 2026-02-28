/**
 * @file mainService.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <string>
#include <unordered_map>
#include <memory>
#include <map>

#include "ara/exec/adaptive_application.h"
#include "apps/fc/main_service/service.hpp"
#include "apps/fc/main_service/mainService.hpp"
#include "core/common/condition.h"
#include "core/rocket_machine_state/rocket_state.hpp"
namespace srp {
namespace service {
namespace {
constexpr auto kService_ipc_name = "srp/apps/MainService/MainService_ipc";
constexpr auto kService_udp_name = "srp/apps/RecoveryService/MainService_udp";
const auto kMain_loop_delay_ms = 10;
const auto kSend_event_time = 1000;
}
using RocketState_t = core::rocketState::RocketState_t;

MainService::MainService() {}

int MainService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
    state_ctr = core::rocketState::RocketStateController::GetInstance();

    service_ipc = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_ipc_name));
    service_udp = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_udp_name));

    state_ctr->RegisterCallback(RocketState_t::ARM, [this]() {
        this->OnArm();
    });
    state_ctr->RegisterOnStateChangeCallback([this](core::rocketState::RocketState_t state) {
            this->OnStateChange(state);
    });
    return 0;
}

void MainService::OnStateChange(core::rocketState::RocketState_t state) {
    service_ipc->CurrentModeStatusEvent.Update(static_cast<uint8_t>(state));
    service_udp->CurrentModeStatusEvent.Update(static_cast<uint8_t>(state));
}

int MainService::Run(const std::stop_token& token) {
    service_ipc->StartOffer();
    service_udp->StartOffer();
    state_ctr->SetState(RocketState_t::DISARM);

    service_ipc->StopOffer();
    service_udp->StopOffer();
    return 0;
}

void MainService::OnArm() {
}

}  // namespace service
}  // namespace srp
