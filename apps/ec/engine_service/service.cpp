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
}


ara::core::Result<bool> MyEngineServiceSkeleton::SetMode(const std::uint8_t& in_parm) {
    if (state_ctr_->SetState(static_cast<core::rocketState::RocketState_t>(in_parm))) {
        return ara::core::Result<bool>{true};
    }
    return ara::core::Result<bool>{false};
}

}  // namespace service
}  // namespace apps
}  // namespace srp
