/**
 * @file service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include "apps/ec/engine_service/service.hpp"
#include "ara/log/log.h"

namespace srp {
namespace apps {
namespace service {

MyEngineServiceSkeleton::MyEngineServiceSkeleton(ara::core::InstanceSpecifier instance):
                                                            EngineServiceSkeleton{instance} {
    state_ctr_ = core::rocketState::RocketStateController::GetInstance();
    vent_ctr_ = engine::VentController::GetInstance();
}


ara::core::Result<bool> MyEngineServiceSkeleton::SetMode(const std::uint8_t& in_parm) {
    if (state_ctr_->SetState(static_cast<core::rocketState::RocketState_t>(in_parm))) {
        return ara::core::Result<bool>{true};
    }
    return ara::core::Result<bool>{false};
}
ara::core::Result<std::uint8_t> MyEngineServiceSkeleton::GetMode() {
    return ara::core::Result<uint8_t>{state_ctr_->GetState()};
}

ara::core::Result<void> MyEngineServiceSkeleton::SetVentValve(const std::uint8_t& in_parm) {
    auto req_state = static_cast<engine::VentState_e>(in_parm);
    vent_ctr_->ChangeState(req_state);
}

}  // namespace service
}  // namespace apps
}  // namespace srp
