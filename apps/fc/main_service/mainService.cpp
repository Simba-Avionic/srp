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
    static constexpr auto kService_ipc_name = "srp/apps/MainApp/MainService_ipc";
    static constexpr auto kService_udp_name = "srp/apps/MainApp/MainService_udp";
    static constexpr auto kSend_event_time =  std::chrono::milliseconds(1000);
    static constexpr auto kArmPinID =  5;
    static constexpr auto kPin_on =    1;
    static constexpr auto kPin_off =   0;
    static constexpr auto kRecovery_instance_name = "srp/apps/MainApp/RecoveryService";
    static constexpr auto kEngine_instance_name =   "srp/apps/MainApp/EngineService";
    static constexpr auto kHeartBeatPinID = 1;
}  // namespace
using RocketState_t = core::rocketState::RocketState_t;

MainService::MainService(): recovery_proxy_{ara::core::InstanceSpecifier{kRecovery_instance_name}},
                            engine_proxy_{ara::core::InstanceSpecifier{kEngine_instance_name}} {}

int MainService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
    apogee_detection_allowed = false;
    state_ctr = core::rocketState::RocketStateController::GetInstance();

    service_ipc = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_ipc_name));
    service_udp = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_udp_name));

    state_ctr->RegisterCallback(RocketState_t::ARM, [this]() { this->OnArm(); });
    state_ctr->RegisterCallback(RocketState_t::DISARM, [this]() { this->OnDisarm(); });
    state_ctr->RegisterCallback(RocketState_t::ABORT, [this]() { this->OnAbort(); });
    state_ctr->RegisterCallback(RocketState_t::LAUNCH, [this]() { this->OnLaunch(); });
    state_ctr->RegisterCallback(RocketState_t::APOGEE, [this]() { this->OnApogee(); });
    state_ctr->RegisterCallback(RocketState_t::SECOND_PARACHUTE, [this]() { this->OnSecondParachute(); });
    state_ctr->RegisterOnStateChangeCallback([this](core::rocketState::RocketState_t state) {
            this->OnStateChange(state);
    });
    state_ctr->RegisterRequirementsCallback([this](core::rocketState::RocketState_t state) {
        if (state == RocketState_t::APOGEE) {
            return apogee_detection_allowed;
        }
        return true;
    });
    recovery_proxy_.StartFindService([this](auto handler) {
        this->recovery_handler_ = handler;
    });
    engine_proxy_.StartFindService([this](auto handler) {
        this->engine_handler = handler;
    });
    service_ipc->StartOffer();
    service_udp->StartOffer();
    return 0;
}

void MainService::OnStateChange(core::rocketState::RocketState_t state) {
    service_ipc->CurrentModeStatusEvent.Update(static_cast<uint8_t>(state));
    service_udp->CurrentModeStatusEvent.Update(static_cast<uint8_t>(state));
    ara::log::LogInfo() << "State changed to: " << core::rocketState::to_string(state);

    auto update_req = false;
    const auto change_require = {RocketState_t::APOGEE,
        RocketState_t::FIRST_PARACHUTE, RocketState_t::SECOND_PARACHUTE, RocketState_t::DROP};
    for (const auto& st : change_require) {
        if (st == state) {
            update_req = true;
            break;
        }
    }
    if (update_req) {
        if (!engine_handler) {
            ara::log::LogWarn() << "Engine Handler not set";
            return;
        }
        engine_handler->SetMode(static_cast<uint8_t>(state));
    }
}

int MainService::Run(const std::stop_token& token) {
    state_ctr->SetState(RocketState_t::DISARM);
    while (!token.stop_requested()) {
        if (gpio_.SetPinValue(kHeartBeatPinID, kPin_on, 500) != core::ErrorCode::kOk) {
            ara::log::LogWarn() << "EngineApp::Run: Failed to toggle heartbeat pin";
        }
        auto state = static_cast<uint8_t>(state_ctr->GetState());
        service_ipc->CurrentModeStatusEvent.Update(state);
        service_udp->CurrentModeStatusEvent.Update(state);
        core::condition::wait_for(kSend_event_time, token);
    }
    service_ipc->StopOffer();
    service_udp->StopOffer();
    recovery_proxy_.StopFindService();
    return 0;
}

void MainService::OnArm() {
    gpio_.SetPinValue(kArmPinID, kPin_on);
    // TOFIX() here should be Linecutter ARM pin but it dont exist...
}

void MainService::OnDisarm() {
    gpio_.SetPinValue(kArmPinID, kPin_off);
    // TOFIX() here should be Linecutter ARM pin but it dont exist...
}

void MainService::OnAbort() {
    this->OnDisarm();
}

void MainService::OnLaunch() {
    apogee_detection_allowed = true;
}

void MainService::OnApogee() {
    // if (!recovery_handler_) {
    //     return;
    // }
    // recovery_handler_->OpenReefedParachute();
    state_ctr->SetState(RocketState_t::FIRST_PARACHUTE);
}

void MainService::OnSecondParachute() {
    // if (!recovery_handler_) {
    //     return;
    // }
    // recovery_handler_->UnreefeParachute();
    state_ctr->SetState(RocketState_t::DROP);
}



}  // namespace service
}  // namespace srp
