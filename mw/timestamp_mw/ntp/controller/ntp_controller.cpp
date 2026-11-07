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
#include "ara/log/log.h"

namespace srp {
namespace tinyNTP {
namespace {
    constexpr auto kRX_Tx_udp_port = 9998;
    constexpr auto kRX_Tx_multicast_port = 9999;
    constexpr auto kMulticastIP = "231.255.42.99";
    constexpr auto kHeader_size = 33;
    constexpr auto kHoldoverTimeoutMs = 5000;
}

bool NtpController::Init(const NtpConfig& config) {
    ntp_config_ = config;
    last_sync_ = GetTimestamp();

    timestamp_.Init();
    discovery_manager_.Init(config.ip, config.ntp_class, is_holdover_);

    const srp::com::soc::SocketConfig udp_sock_config = srp::com::soc::SocketConfig(
                                        config.ip, kRX_Tx_udp_port, kRX_Tx_udp_port);
    if (udp_sock_.Init(udp_sock_config) != srp::core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize udp socket!";
    } else {
        ara::log::LogInfo() << "Udp socket initialized!";
    }

    const srp::com::soc::SocketConfig multicast_sock_config = srp::com::soc::SocketConfig(
                                        config.ip, kRX_Tx_multicast_port, kRX_Tx_multicast_port);
    if (multicast_sock_.Init(multicast_sock_config) != srp::core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to initialize multicast socket!";
    } else {
        ara::log::LogInfo() << "Multicast socket initialized!";
    }

    this->udp_sock_.SetRXCallback([this](const std::string& ip, const uint16_t& port,
        const std::vector<uint8_t>& payload) {
            udp_socket_callback(ip, port, payload);
        });
    this->multicast_sock_.SetRXCallback([this](const std::string& ip, const uint16_t& port,
        const std::vector<uint8_t>& payload) {
            multicast_socket_callback(ip, port, payload);
    });

    udp_sock_.StartRXThread();
    multicast_sock_.StartRXThread();

    ntp_thread = std::jthread([this](std::stop_token token){
        thread_loop(token);
    });

    ara::log::LogInfo() << "NtpController initialized with IP: " << ntp_config_.ip
                        << ", NTP Class: " << static_cast<int>(ntp_config_.ntp_class)
                        << ", interval [ms]: " << ntp_config_.t_hb_ms;

    return true;
}

int64_t NtpController::CalculateOffset(const int64_t& T0, const int64_t& T1,
    const int64_t& T2, const int64_t& T3) {
    return ((T1 - T0) + (T2 - T3)) / 2;
}

uint64_t NtpController::CalculateRoundTripDelay(const int64_t& T0, const int64_t& T1,
            const int64_t& T2, const int64_t& T3) {
    return static_cast<uint64_t>((T3 - T0) - (T2 - T1));
}

uint8_t NtpController::EncodeSettings(const uint8_t device_class, const bool is_holdover, const uint8_t msg_type) {
    uint8_t settings = 0;

    // Bity 0-2: Klasa urządzenia
    settings |= (device_class & 0x07);

    // Bit 3: Holdover
    if (is_holdover) {
        settings |= (1 << 3);
    }

    // Bity 4-5: Version (currently 00)
    settings |= (0x00 << 4);
    // Bit 6: msg_type (0 dla Unicast, 1 dla Announce)
    if (msg_type == 1) {
        settings |= (1 << 6);
    }

    // Bit 7: Reserved

    return settings;
}

void NtpController::SendAnnounce() {
    srp::mw::tinyNTP::ntpStruct frame;

    frame.settings = EncodeSettings(ntp_config_.ntp_class, is_holdover_, 1);
    frame.t0 = 0; frame.t1 = 0; frame.t2 = 0; frame.t3 = 0;

    const auto buf = srp::data::Convert2Vector<srp::mw::tinyNTP::ntpStruct>::Conv(frame);

    if (multicast_sock_.Transmit(ntp_config_.ip, kRX_Tx_multicast_port, buf) != srp::core::ErrorCode::kOk) {
        ara::log::LogError() << "Failed to send Announce multicast frame!";
    } else {
        ara::log::LogDebug() << "Announce multicast frame sent correctly!";
    }
}

void NtpController::SendSyncRequest(const std::string& current_master_ip) {
    srp::mw::tinyNTP::ntpStruct header;
    header.settings = EncodeSettings(ntp_config_.ntp_class, is_holdover_, 0);
    header.t0 = GetTimestamp();
    header.t1 = 0; header.t2 = 0; header.t3 = 0;

    last_t0_ = header.t0;

    const auto buf = srp::data::Convert2Vector<srp::mw::tinyNTP::ntpStruct>::Conv(header);

    this->udp_sock_.Transmit(current_master_ip, kRX_Tx_udp_port, buf);
}

std::optional<srp::mw::tinyNTP::ntpStruct> NtpController::ParseAndValidatePayload(
                        const std::vector<uint8_t>& payload, const std::string& ip) {
    if (payload.size() != kHeader_size) {
        ara::log::LogError() << "Invalid payload size! Rejecting the packet.";
        return std::nullopt;
    }

    if (ip == ntp_config_.ip) {
        ara::log::LogDebug() << "Rejecting own packet.";
        return std::nullopt;
    }

    return srp::data::Convert<srp::mw::tinyNTP::ntpStruct>::Conv(payload);
}

void NtpController::udp_socket_callback(const std::string& ip,
                                    const uint16_t& port,
                                    const std::vector<uint8_t>& payload) {
    const int64_t now_ms = GetTimestamp();

    const auto val = ParseAndValidatePayload(payload, ip);
    if (!val.has_value()) return;
    srp::mw::tinyNTP::ntpStruct header = val.value();

    const uint8_t msg_type = (header.settings >> 6) & 0x01;

    if (msg_type != 0) {
        ara::log::LogWarn() << "Received non-Sync message on unicast socket from: " << ip;
        return;
    }

    const auto master_opt = discovery_manager_.GetBestMaster();
    const bool is_server = (!master_opt.has_value() || master_opt.value().ip == ntp_config_.ip);

    if (is_server) {
        if (header.t1 != 0 || header.t2 != 0) {
            ara::log::LogWarn() << "Received Sync response, but I am the server now. Dropping.";
            return;
        }

        header.t1 = now_ms;
        header.t2 = GetTimestamp();

        const auto buf = srp::data::Convert2Vector<srp::mw::tinyNTP::ntpStruct>::Conv(header);

        udp_sock_.Transmit(ip, kRX_Tx_udp_port, buf);

        ara::log::LogDebug() << "Sent sync response to " << ip;
    } else {
        if (header.t1 == 0 || header.t2 == 0) {
            ara::log::LogWarn() << "Received Sync request, but I am a client now. Dropping.";
            return;
        }

        if (header.t0 != last_t0_) {
            ara::log::LogWarn() << "Received Sync response for an old or unknown request. Dropping.";
            return;
        }

        const int64_t t3 = now_ms;

        const auto offset = CalculateOffset(header.t0, header.t1, header.t2, t3);
        const auto round_trip_time = CalculateRoundTripDelay(header.t0, header.t1, header.t2, t3);

        this->timestamp_.CorrectStartPoint(offset);

        is_holdover_ = master_opt.value().holdover;
        discovery_manager_.SetLocalNodeHoldover(is_holdover_);
        last_sync_ = now_ms;

        ara::log::LogDebug() << "Round trip time [ms]: " << round_trip_time
                                << " ,offset value [ms]: " << offset;
    }
}

void NtpController::multicast_socket_callback(const std::string& ip,
                                    const uint16_t& port,
                                    const std::vector<uint8_t>& payload) {
    const auto val = ParseAndValidatePayload(payload, ip);
    if (!val.has_value()) return;
    const srp::mw::tinyNTP::ntpStruct header = val.value();

    const uint8_t msg_type = (header.settings >> 6) & 0x01;

    if (msg_type != 1) {
        ara::log::LogWarn() << "Received non-Announce message on multicast socket from: " << ip;
        return;
    }

    const uint8_t sender_class = header.settings & 0x07;
    const bool holdover = (header.settings >> 3) & 0x01;

    ara::log::LogDebug() << "Updating node with ip: " << ip;
    discovery_manager_.UpdateNode(ip, sender_class, holdover);
}

int64_t NtpController::GetTimestamp() {
    return this->timestamp_.GetNewTimeStamp();
}

void NtpController::thread_loop(std::stop_token token) {
    ara::log::LogInfo() << "Start NTP Sync.";

    while (!token.stop_requested()) {
        const auto master_opt = discovery_manager_.GetBestMaster();

        SendAnnounce();

        if (!master_opt.has_value() || master_opt.value().ip == ntp_config_.ip) {
            ara::log::LogDebug() << "Working as a server";
            if (is_holdover_) {
                is_holdover_ = false;
                discovery_manager_.SetLocalNodeHoldover(is_holdover_);
                ara::log::LogDebug() << "Node time is now treated as accurate.";
            }
        } else {
            const int64_t current_time = GetTimestamp();
            if (current_time - last_sync_ > kHoldoverTimeoutMs) {
                is_holdover_ = true;
                discovery_manager_.SetLocalNodeHoldover(is_holdover_);
            }

            ara::log::LogDebug() << "Sending Sync to Master: " << master_opt.value().ip;
            SendSyncRequest(master_opt.value().ip);
        }

        core::condition::wait_for(std::chrono::milliseconds(ntp_config_.t_hb_ms), token);
    }
}

}  // namespace tinyNTP
}  // namespace srp
