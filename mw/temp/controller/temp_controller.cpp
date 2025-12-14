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
}

srp::core::ErrorCode TempController::Init(uint16_t service_id, std::unique_ptr<com::soc::ISocket> sock) {
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
    this->sub_sock_->StartRXThread();
    return res;
}

srp::core::ErrorCode TempController::Subscribe(std::optional<std::string> name) {
    srp::mw::temp::TempSubHdr hdr{this->service_id, name};
    auto buf = srp::data::Convert2Vector<srp::mw::temp::TempSubHdr>::Conv(hdr);
    if (auto res = sub_sock_->Transmit(kTempServiceName, 0, buf)) {
        ara::log::LogError() <<("Failed to subscribe to " + std::string(kTempServiceName)+":::"+std::to_string(res));
        return res;
    }
    return srp::core::ErrorCode::kOk;
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
    srp::com::soc::RXCallback lambdaCallback = [this](
        const std::string& ip, const std::uint16_t& port,
            const std::vector<std::uint8_t> data) {
        auto hdr = this->Conv(data);
        this->callback_(hdr);
    };
    this->sub_sock_->SetRXCallback(lambdaCallback);
}

srp::core::ErrorCode TempController::Initialize(uint16_t service_id,
                TempRXCallback callback, std::unique_ptr<com::soc::ISocket> sock) {
    if (this->Init(service_id, std::move(sock)) != core::ErrorCode::kOk) {
        return core::ErrorCode::kInitializeError;
    }
    return this->SetUp(std::move(callback));
}
}  // namespace temp
}  // namespace mw
}  // namespace srp
