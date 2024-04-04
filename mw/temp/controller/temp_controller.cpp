/**
 * @file temp_controller.cpp
 * @author Maciek Matuszewski (maciej.matuszewsky@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "temp_controller.h"
#include <cstring>
#include <sys/types.h>  // NOLINT
#include <sys/socket.h>  // NOLINT
#include <netinet/in.h>  // NOLINT
#include <iostream>

namespace simba {
namespace mw {
namespace temp {

namespace {
    static constexpr char const*
        kTempServiceName = "SIMBA.TEMP.SERVICE";
    static constexpr char const*
        kSubscriberPrefix = "SIMBA.TEMP.";
}

simba::core::ErrorCode TempController::Init(
    uint16_t service_id, simba::com::soc::RXCallback callback) {
    this->service_id = service_id;
    if (auto ret = this->sub_sock_.Init(
        com::soc::SocketConfig(
            kSubscriberPrefix + std::to_string(this->service_id), 0, 0))) {
        AppLogger::Error("Couldn't initialize socket!");
        return ret;
    }
    this->callback_ = callback;
    SetTempRXCallback();
    this->Subscribe();
    this->sub_sock_.StartRXThread();
    return simba::core::ErrorCode::kOk;
}

simba::core::ErrorCode TempController::Subscribe() {
    simba::mw::temp::SubMsgFactory factory;
    SubscribeHeader hdr{this->service_id};
    std::vector<uint8_t> data =
        factory.GetBuffer(std::make_shared<SubscribeHeader>(hdr), {});
    if (sub_sock_.Transmit(kTempServiceName, 0, data) != core::ErrorCode::kOk) {
        AppLogger::Error("Failed to subscribe to " + std::string(kTempServiceName));
        return core::ErrorCode::kError;
    }
    return simba::core::ErrorCode::kOk;
}

void TempController::SetTempRXCallback() {
    simba::com::soc::RXCallback lambdaCallback = [this](
        const std::string& ip, const std::uint16_t& port,
            const std::vector<std::uint8_t> data) {
        this->callback_(ip, port, data);
    };
    this->sub_sock_.SetRXCallback(lambdaCallback);
}

}  // namespace temp
}  // namespace mw
}  // namespace simba
