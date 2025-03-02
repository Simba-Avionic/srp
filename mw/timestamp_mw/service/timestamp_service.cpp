/**
 * @file timestamp_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <memory>
#include "mw/timestamp_mw/service/timestamp_service.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"

namespace srp {
namespace mw {

int TimestampService::Run(const std::stop_token& token) {
    core::condition::wait(token);
    return 1;
}
int TimestampService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                    parms) {
    ara::log::LogWarn() << "Starting app";
    if (!this->ntp_controller.Init()) {
        ara::log::LogError() << "NTP controller initialization failed.";
        return -1;
    }
    ara::log::LogWarn() << "Init completed";
    return 0;
}

}  // namespace mw
}  // namespace srp
