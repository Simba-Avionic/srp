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
    auto val = srp::data::Convert<srp::mw::tinyNTP::ntpStruct>::Conv(payload);
    if (!val.has_value()) {
        ara::log::LogWarn() << "NTP callback decode failed from " << ip << ":" << port;
        return {};
    }
    mw::tinyNTP::ntpStruct hdr = val.value();
    hdr.t1 = now_ms;
    hdr.t2 = GetTimestamp();
    ara::log::LogDebug() << "NTP callback handled for " << ip << ":" << port;
    return srp::data::Convert2Vector<srp::mw::tinyNTP::ntpStruct>::Conv(hdr);
}


int64_t NtpController::GetTimestamp() {
    return this->timestamp_.GetNewTimeStamp();
}

void NtpController::thread_loop(std::stop_token token) {
    while (!token.stop_requested()) {
        ara::log::LogDebug() << "Starting NTP sync cycle";
        srp::mw::tinyNTP::ntpStruct header;
        header.t0 = GetTimestamp();
        auto buf = srp::data::Convert2Vector<srp::mw::tinyNTP::ntpStruct>::Conv(header);
        auto res = this->sock_.Transmit(masterIP, kRX_port, buf);
        auto t3 = GetTimestamp();
        if (!res.has_value()) {
            ara::log::LogWarn() << "NTP sync transmit failed to " << masterIP << ":" << kRX_port;
            core::condition::wait_for(std::chrono::milliseconds(kDelay_time), token);
            continue;
        }
        auto val = srp::data::Convert<srp::mw::tinyNTP::ntpStruct>::Conv(res.value());
        if (!val.has_value()) {
            ara::log::LogWarn() << "NTP sync response decode failed";
            core::condition::wait_for(std::chrono::milliseconds(kDelay_time), token);
            continue;
        }
        srp::mw::tinyNTP::ntpStruct hdr = val.value();
        auto offset = CalculateOffset(hdr.t0, hdr.t1, hdr.t2, t3);
        auto round_trip_time = CalculateRoundTripDelay(hdr.t0, hdr.t1, hdr.t2, t3);
        this->timestamp_.CorrectStartPoint(offset);
        ara::log::LogDebug() << "Round trip time [ms]: " << round_trip_time
                << " ,offset value [ms]: " << offset;
        core::condition::wait_for(std::chrono::milliseconds(kDelay_time), token);
    }
}

bool NtpController::Init() {
    auto ip = readMyIP();
    if (!ip.has_value()) {
        ara::log::LogError() << "NTP init failed: cannot read local IP";
        return false;
    }
    myIP = ip.value();
    timestamp_.Init();
    if (myIP == masterIP) {
        ara::log::LogInfo() << "NTP running in master mode on " << myIP;
        if (sock_.Init(com::soc::SocketConfig{myIP, kRX_port, kTx_port}) != core::ErrorCode::kOk) {
            ara::log::LogError() << "NTP init failed: socket init error for master mode";
            return false;
        }
        this->sock_.SetRXCallback(std::bind(&NtpController::socket_callback, this, std::placeholders::_1,
                                                       std::placeholders::_2, std::placeholders::_3));
        sock_.StartRXThread();
    } else {
        ara::log::LogInfo() << "NTP running in client mode, local IP: " << myIP
                            << ", master: " << masterIP;
        ntp_thread = std::jthread([this](std::stop_token token){
            thread_loop(token);
        });
    }
    ara::log::LogInfo() << "NTP controller initialized";
    return true;
}

// TODO(matikrajek42@gmail.com)  fix this shit func
std::optional<std::string> NtpController::readMyIP() {
    const std::string path = kIp_file_path;
    auto parser = core::json::JsonParser::Parser(path);
    if (!parser.has_value()) {
        ara::log::LogError() << "readMyIP failed: cannot parse " << path;
        return std::nullopt;
    }
    auto ip = parser.value().GetString("ip");
    if (!ip.has_value()) {
        ara::log::LogError() << "readMyIP failed: missing 'ip' in " << path;
        return std::nullopt;
    }
    return ip.value();
}

}  // namespace tinyNTP
}  // namespace srp
