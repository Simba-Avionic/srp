/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/engine_service/engine_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace simba {
namespace apps {

EngineApp::EngineApp(): primer_(ara::core::InstanceSpecifier{"simba/apps/PrimerService/PrimService_ipc"}),
                            servo_((ara::core::InstanceSpecifier{"simba/apps/servoService/ServoService_ipc"})) {
}


int EngineApp::Run(const std::stop_token& token) {
    ara::log::LogInfo() << "Run complete, closing";
    core::condition::wait(token);
}

int EngineApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    ara::log::LogInfo() << "Initialize Complete";
}


}  // namespace apps
}  // namespace simba
