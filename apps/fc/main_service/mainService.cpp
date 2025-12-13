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
#include "apps/fc/main_service/rocket_state.h"
namespace srp {
namespace service {
namespace {
constexpr auto kService_ipc_name = "srp/apps/MainService/MainService_ipc";
constexpr auto kService_udp_name = "srp/apps/RecoveryService/MainService_udp";
const auto kMain_loop_delay_ms = 10;
const auto kSend_event_time = 1000;
}
using RocketState_t = apps::RocketState_t;

MainService::MainService() {}

int MainService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
    service_ipc = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_ipc_name));
    service_udp = std::make_unique<apps::MyMainServiceSkeleton>(
        ara::core::InstanceSpecifier(kService_udp_name));
    controller.Init();
    service_ipc->StartOffer();
    service_udp->StartOffer();
    return 0;
}
int MainService::Run(const std::stop_token& token) {
    auto last_send = std::chrono::high_resolution_clock::now();
    auto state = apps::RocketState::GetInstance();
    while (!token.stop_requested()) {
        controller.SecureLoop();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - last_send);
        if (elapsed.count() > kSend_event_time) {
            auto val = static_cast<uint8_t>(state->GetState());
            this->service_ipc->CurrentModeStatusEvent.Update(val);
            this->service_udp->CurrentModeStatusEvent.Update(val);
            last_send = std::chrono::high_resolution_clock::now();
        }
        core::condition::wait_for(std::chrono::milliseconds(kMain_loop_delay_ms), token);
    }
    service_ipc->StopOffer();
    service_udp->StopOffer();
    return 0;
}
}  // namespace service
}  // namespace srp
