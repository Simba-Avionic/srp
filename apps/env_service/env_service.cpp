/**
 * @file env_service.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <memory>
#include <map>
#include <utility>
#include "apps/env_service/env_service.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include "mw/temp/temp_reading_msg/temp_reading_msg_factory.h"
#include "simba/env/EnvAppSkeleton.h"

namespace simba {
namespace envService {


core::ErrorCode EnvService::Init(std::unique_ptr<mw::temp::TempController> temp) {
    if (this->temp_ || !temp) {
      return core::ErrorCode::kInitializeError;
    }
    this->temp_ = std::move(temp);
    return core::ErrorCode::kOk;
}

EnvService::EnvService(): service_ipc{ara::core::InstanceSpecifier{"simba/env/EnvApp/envService_ipc"}},
                        service_udp{ara::core::InstanceSpecifier{"simba/env/EnvApp/envService_udp"}} {
}


int EnvService::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
    this->Init(std::make_unique<mw::temp::TempController>());
    core::ErrorCode res;
    uint8_t i = 0;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        res = this->temp_->Initialize(514, std::bind(&EnvService::TempRxCallback,
            this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3), std::make_unique<com::soc::IpcSocket>());
    } while (res != core::ErrorCode::kOk && i < 6);
    service_ipc.StartOffer();
    service_udp.StartOffer();
    return core::ErrorCode::kOk;
}

int EnvService::Run(const std::stop_token& token) {
    int i = 1;
    while (!token.stop_requested()) {
        core::condition::wait_for(std::chrono::milliseconds(1000), token);
        this->service_ipc.newPressEvent.Update(i);
        this->service_udp.newPressEvent.Update(i);
        this->service_ipc.newDPressEvent.Update(i);
        this->service_udp.newDPressEvent.Update(i);
        i += 1;
    }
    service_ipc.StopOffer();
    service_udp.StopOffer();
    return core::ErrorCode::kOk;
}

void EnvService::TempRxCallback(const std::string& ip, const std::uint16_t& port,
                                const std::vector<std::uint8_t> data) {
    mw::temp::TempReadingMsgFactory factory_;
    auto hdrs = factory_.GetPayload(data);
    for (auto &hdr : hdrs) {
        ara::log::LogWarn() << "Receive temp id: " << hdr.first << ",temp:" << static_cast<float>(hdr.second);
        switch (hdr.first) {
            case 0:
            this->service_ipc.newTempEvent_1.Update(static_cast<int16_t>(hdr.second * 10));
            this->service_udp.newTempEvent_1.Update(static_cast<int16_t>(hdr.second * 10));
            break;
            case 1:
            this->service_ipc.newTempEvent_2.Update(static_cast<int16_t>(hdr.second * 10));
            this->service_udp.newTempEvent_2.Update(static_cast<int16_t>(hdr.second * 10));
            break;
            case 2:
            this->service_ipc.newTempEvent_3.Update(static_cast<int16_t>(hdr.second * 10));
            this->service_udp.newTempEvent_3.Update(static_cast<int16_t>(hdr.second * 10));
            break;
            default:
            ara::log::LogWarn() << "ID spoza zakresu:" << hdr.first;
            break;
        }
    }
}


}  // namespace envService
}  // namespace simba
