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
#include <cstring>
#include <sys/types.h>  // NOLINT
#include <sys/socket.h>  // NOLINT
#include <netinet/in.h>  // NOLINT
#include <utility>
#include "mw/temp/controller/temp_controller.h"



namespace srp {
namespace mw {
namespace temp {

namespace {
    constexpr auto kTempServiceName = "SRP.TEMP.SERVICE";
    constexpr auto kSubscriberPrefix = "SRP.TEMP.";
    constexpr int PHYSICAL_ID_SIZE = 15;
}

srp::core::ErrorCode TempController::Init(uint16_t service_id, std::unique_ptr<com::soc::StreamIpcSocket> sock) {
    if (!sock) {
        return core::ErrorCode::kInitializeError;
    }
    this->sub_sock_ = std::move(sock);
    this->service_id = service_id;
    return core::ErrorCode::kOk;
}

srp::core::ErrorCode TempController::SetUp(TempRXCallback callback) {
    auto res = core::ErrorCode::kOk;
    if ((res = this->sub_sock_->Init(
        com::soc::SocketConfig(
            kSubscriberPrefix + std::to_string(this->service_id), 0, 0)))) {
        ara::log::LogError() <<("Couldn't initialize socket!");
        return res;
    }
    this->callback_ = callback;
    SetTempRXCallback();
    if (res != core::ErrorCode::kOk) {
        return res;
    }
    return res;
}

std::optional<uint8_t> TempController::Register(std::string name) {
    auto sensor_id = Subscribe(name);
    this->sub_sock_->StartRXThread();
    return (sensor_id && !sensor_id->empty()) ? std::make_optional(sensor_id->front()) : std::nullopt;
}

std::optional<std::vector<uint8_t>> TempController::Subscribe(std::string name) {
    if (name.size() != PHYSICAL_ID_SIZE) {
        return std::vector<uint8_t>{srp::core::ErrorCode::kError};
    }
    srp::mw::temp::TempSubHdr hdr{this->service_id, name[3], name[4],
                                name[5], name[6], name[7], name[8],
                                name[9], name[10], name[11], name[12],
                                name[13], name[14]};
    auto buf = srp::data::Convert2Vector<srp::mw::temp::TempSubHdr>::Conv(hdr);
    std::optional<std::vector<uint8_t>> res;
    if (res = sub_sock_->Transmit(kTempServiceName, 0, buf)) {
        return res;
    }
    ara::log::LogError() << "Failed to subscribe to " << std::string(kTempServiceName);
    return res;
}

std::vector<srp::mw::temp::TempReadHdr> TempController::Conv(const std::vector<uint8_t>& data) const {
    std::vector<srp::mw::temp::TempReadHdr> readings;
    size_t offset = 0;
    while (offset + 5 <= data.size()) {
        auto parsed = srp::data::Convert<srp::mw::temp::TempReadHdr>::Conv(
            std::vector<uint8_t>{data.begin() + offset, data.begin() + offset + 5});
        if (!parsed.has_value()) {
            break;
        }
        readings.push_back(parsed.value());
        offset += 5;
    }
    return readings;
}

void TempController::SetTempRXCallback() {
    srp::com::soc::RXCallbackStream lambdaCallback = [this](
        const std::string& ip, const std::uint16_t& port,
            const std::vector<std::uint8_t> data) {
        auto hdr = this->Conv(data);
        this->callback_(hdr);
        return std::vector<uint8_t>{};
    };
    this->sub_sock_->SetRXCallback(lambdaCallback);
}

srp::core::ErrorCode TempController::Initialize(uint16_t service_id,
                TempRXCallback callback, std::unique_ptr<com::soc::StreamIpcSocket> sock) {
    if (this->Init(service_id, std::move(sock)) != core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }
    return this->SetUp(std::move(callback));
}
}  // namespace temp
}  // namespace mw
}  // namespace srp
