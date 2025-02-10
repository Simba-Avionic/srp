/**
 * @file ntp_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "mw/timestamp_mw/ntp/controller/ntp_controller.hpp"
#include <utility>
#include <vector>
#include "core/common/condition.h"
#include "core/json/json_parser.h"
#include "ara/log/log.h"
namespace srp {
namespace tinyNTP {
namespace {
    constexpr auto kIp_file_path = "/srp/opt/cpu_srp/logger_config.json";
    constexpr auto kRX_port = 9999;
    constexpr auto kTx_port = 9998;
    constexpr auto masterIP = "192.168.10.102";
    constexpr auto kHeader_size = 33;
    constexpr auto kDelay_time = 4000;
}

int64_t NtpController::CalculateOffset(const int64_t T0, const int64_t T1,
    const int64_t T2, const int64_t T3) {
return ((T1 - T0) + (T2 - T3)) / 2;
}

uint64_t NtpController::CalculateRoundTripDelay(const int64_t T0, const int64_t T1,
            const int64_t T2, const int64_t T3) {
return static_cast<uint64_t>((T3 - T0) - (T2 - T1));
}


std::vector<uint8_t> NtpController::socket_callback(const std::string& ip, const std::uint16_t& port,
                                                                const std::vector<std::uint8_t> payload) {
    auto now_ms = GetTimestamp();
    ara::log::LogDebug() << "Receive socket callback";
    if (payload.size() != kHeader_size) {
        return {};
    }
    tinyNTP::NtpHeader header;
    header.SetBuffor(payload);
    header.SetSettings(2, tinyNTP::NTPMode_t::SERVER);
    header.SetT1Timestamp(now_ms);
    ara::log::LogDebug() << "Success send response for callback";
    header.SetT2Timestamp(GetTimestamp());
    return header.GetBuffor();
}


int64_t NtpController::GetTimestamp() {
    return this->timestamp_.GetNewTimeStamp();
}

void NtpController::thread_loop(std::stop_token token) {
    while (!token.stop_requested()) {
        core::condition::wait_for(std::chrono::milliseconds(kDelay_time), token);
        ara::log::LogDebug() << "Start NTP SYNC";
        tinyNTP::NtpHeader header;
        header.SetSettings(4, tinyNTP::NTPMode_t::CLIENT);
        header.SetT0Timestamp(GetTimestamp());
        auto res = this->sock_.Transmit(masterIP, kRX_port, header.GetBuffor());
        auto t3 = GetTimestamp();
        if (!res.has_value()) {
            continue;
        }
        if (res.value().size() != kHeader_size) {
            continue;
        }
        header.SetBuffor(res.value());
        auto offset = CalculateOffset(header.GetT0Timestamp(), header.GetT1Timestamp(),
                                                    header.GetT2Timestamp(), t3);
        auto round_trip_time = CalculateRoundTripDelay(header.GetT0Timestamp(),
                        header.GetT1Timestamp(), header.GetT2Timestamp(), t3);
        this->timestamp_.CorrectStartPoint(offset);
        ara::log::LogInfo() << "Round trip time [ms]: " << std::to_string(round_trip_time)
                << " ,offset value [ms]: " << std::to_string(static_cast<int>(offset));
    }
}

bool NtpController::Init() {
    auto ip = readMyIP();
    if (!ip.has_value()) {
        return false;
    }
    myIP = ip.value();
    timestamp_.Init();
    if (myIP == masterIP) {
        sock_.Init(com::soc::SocketConfig{myIP, kRX_port, kTx_port});
        this->sock_.SetRXCallback(std::bind(&NtpController::socket_callback, this, std::placeholders::_1,
                                                       std::placeholders::_2, std::placeholders::_3));
        sock_.StartRXThread();
    } else {
        ntp_thread = std::jthread([this](std::stop_token token){
            thread_loop(token);
        });
    }
    return true;
}

// TODO(matikrajek42@gmail.com)  fix this shit func
std::optional<std::string> NtpController::readMyIP() {
    const std::string path = kIp_file_path;
    auto parser = core::json::JsonParser::Parser(path);
    if (!parser.has_value()) {
        return std::nullopt;
    }
    auto ip = parser.value().GetString("ip");
    if (!ip.has_value()) {
        return std::nullopt;
    }
    return ip.value();
}

}  // namespace tinyNTP
}  // namespace srp
