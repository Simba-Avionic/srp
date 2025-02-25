/**
 * @file exec_manager.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <vector>
#include <iostream>
#include <algorithm>

#include "exec_manager.hpp"

namespace srp {
namespace mw {
namespace exec {

std::pair<srp::diag::exec::Status, std::bitset<5>> ExecManager::getStatusAndFlags(uint8_t data) {
    srp::diag::exec::Status status_ = srp::diag::exec::Status(data & 0b00000111);
    std::bitset<8> bits(data);
    std::bitset<5> flags(bits.to_string().substr(0, 5));
    return {status_, flags};
}

void ExecManager::RxCallback(const std::string& ip, const std::uint16_t& port,
                std::vector<std::uint8_t> payload) {
    if ( payload.size() <=0 ) { return; }
    std::lock_guard<std::mutex> lock(mtx);
    diag::exec::ExecHeader hdr{};
    hdr.SetBuffor(payload);
        AppLogger::Debug("Receive hb:"+std::to_string(hdr.GetServiceID())+
                    ", timestamp:"+std::to_string(hdr.GetTimestamp()));
    auto it = this->db_.find(hdr.GetServiceID());
    if (it == this->db_.end()) {
        AppLogger::Warning("Invalid service id:"+std::to_string(hdr.GetServiceID()));
        return;
    }
    auto pair = this->getStatusAndFlags(hdr.GetFlags());
    it->second.last_timestamp_time = std::chrono::high_resolution_clock::now();
    if (it->second.last_timestamp+1 != hdr.GetTimestamp() && it->second.last_timestamp != 0) {
        AppLogger::Warning("Missing timestamp service_id: "+std::to_string(hdr.GetServiceID()));
    }
    if ( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                                                            - it->second.last_timestamp_time).count() > 1250 ) {
        it->second.invalid_timestamp_num++;
    }
    it->second.last_timestamp = hdr.GetTimestamp();
    if ( it->second.state != pair.first ) {
        AppLogger::Info("change app "+std::to_string(it->first)+" status to "+std::to_string(it->second.state));
        it->second.state = pair.first;
    }
    if (it->second.flags != pair.second) {
        it->second.flags = pair.second;
    }
}

void ExecManager::SetApps(std::vector<srp::em::service::data::AppConfig> apps) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto app : apps) {
        this->db_.insert({app.GetAppID(), Service(0)});
    }
}

std::queue<uint16_t> ExecManager::CheckAppCondition() {
  std::lock_guard<std::mutex> lock(mtx);
  std::queue<uint16_t> res;
  for ( auto &app : this->db_ ) {
    if ( app.second.last_timestamp == 0 || app.second.execState == ExecState::kRestartNeeded ) {
        continue;
    }
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - app.second.last_timestamp_time).count();
    if ( (delta > 5000 || app.second.invalid_timestamp_num > 5)
                                        && app.second.execState != ExecState::kRestartNeeded && app.first != 258) {
        AppLogger::Error("app need restart[][][][][][][][][][][]");
        res.push(app.first);
        app.second.execState = ExecState::kRestartNeeded;
    }
  }
  return res;
}

void ExecManager::Init() {
    if (this->sock_.Init(com::soc::SocketConfig{"SRP.EXE", 0, 0}) != core::ErrorCode::kOk) {
        AppLogger::Error("failed to bind ipc");
    } else {
        AppLogger::Debug("ipc bind succesfully");
    }
    this->sock_.SetRXCallback(
        std::bind(&ExecManager::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    AppLogger::Debug("finish startup exec");
}

void ExecManager::RestartedApp(uint16_t appID) {
    for (auto& app : this->db_) {
        if (app.first == appID) {
            app.second.execState = ExecState::kRunning,
            app.second.state = diag::exec::Status::Start_up;
            app.second.last_timestamp = 0;
            app.second.flags = 0;
            app.second.invalid_timestamp_num = 0;
        }
    }
}

}  // namespace exec
}  // namespace mw
}  // namespace srp
