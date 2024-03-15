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

namespace simba {
namespace mw {
namespace exec {

std::pair<simba::diag::exec::Status, std::bitset<5>> ExecManager::getStatusAndFlags(uint8_t data) {
    simba::diag::exec::Status status_ = simba::diag::exec::Status(data & 0b00000111);
    std::bitset<8> bits(data);
    std::bitset<5> flags(bits.to_string().substr(0, 5));
    return {status_, flags};
}

void ExecManager::RxCallback(const std::string& ip, const std::uint16_t& port,
                std::vector<std::uint8_t> payload) {
    if ( payload.size() <=0 ) { return; }
    diag::exec::ExecHeader hdr{};
    hdr.SetBuffor(payload);
    AppLogger::Info("Receive hb:"+std::to_string(hdr.GetServiceID())+
                    ", timestamp:"+std::to_string(hdr.GetTimestamp()));
    mtx.lock();
    auto it = this->db_.find(hdr.GetServiceID());
    auto pair = this->getStatusAndFlags(hdr.GetFlags());
    if (it == this->db_.end()) {
        AppLogger::Warning("Invalid service id:"+std::to_string(hdr.GetServiceID()));
        this->db_.insert({hdr.GetServiceID(), Service(hdr.GetTimestamp(), pair.first, pair.second)});
        AppLogger::Warning("Registered new hb to tracking");
        return;
    }
    if (it->second.last_timestamp+1 != hdr.GetTimestamp()) {
        AppLogger::Error("Missing timestamp service_id: "+std::to_string(hdr.GetServiceID()));
    }
    if ( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()
                                                            - it->second.last_timestamp_time).count() > 1250 ) {
        it->second.invalid_timestamp_num++;
    }
    it->second.last_timestamp_time = std::chrono::high_resolution_clock::now();
    it->second.last_timestamp = hdr.GetTimestamp();
    if ( it->second.state != pair.first ) {
        AppLogger::Info("change app "+std::to_string(it->first)+" status to "+std::to_string(it->second.state));
        it->second.state = pair.first;
    }
    if (it->second.flags != pair.second) {
        it->second.flags = pair.second;
    }
    mtx.unlock();
}

void ExecManager::SetApps(std::vector<simba::em::service::data::AppConfig> apps) {
    mtx.lock();
    for (auto app : apps) {
        this->db_.insert({app.GetAppID(), Service(0)});
    }
    mtx.unlock();
}

std::queue<uint16_t> ExecManager::CheckAppCondition() {
  mtx.lock();
  std::queue<uint16_t> res;
  for ( auto &app : this->db_ ) {
    if ( app.second.last_timestamp == 0 ) {
        continue;
    }
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::high_resolution_clock::now() - app.second.last_timestamp_time).count();
    if ( delta > 5000 || app.second.invalid_timestamp_num > 5 ) {
        AppLogger::Error("app need restart[][][][][][][][][][][]");
        res.push(app.first);
        this->db_.erase(app.first);
    }
  }
  mtx.unlock();
  return res;
}

void ExecManager::Init() {
    if (this->sock_.Init(com::soc::SocketConfig{"SIMBA.EXE", 0, 0}) != core::ErrorCode::kOk) {
        AppLogger::Warning("failed to bind ipc");
    } else {
        AppLogger::Debug("ipc bind succesfully");
    }
    this->sock_.SetRXCallback(
        std::bind(&ExecManager::RxCallback, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    this->sock_.StartRXThread();
    AppLogger::Debug("finish startup exec");
}

}  // namespace exec
}  // namespace mw
}  // namespace simba
