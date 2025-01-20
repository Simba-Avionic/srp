/**
 * @file exec_controller.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <mutex>  // NOLINT
#include "exec_controller.hpp"
#include "core/logger/Logger.h"
#include "communication-core/sockets/ipc_socket.h"
#include "diag/exec/data/exec_header.hpp"
namespace srp {
namespace diag {
namespace exec {

void ExecController::Init(uint16_t service_id) {
    this->status_ = Status::Start_up;
    this->flags_ = std::bitset<5>{"00000"};
    this->thread_ = std::jthread([&](std::stop_token stop_token) {
    auto sock_ = com::soc::IpcSocket();
    auto hdr = ExecHeader(service_id, 1, (this->flags_.to_ulong() << 3) | this->status_);
    while (!stop_token.stop_requested()) {
        {
            std::unique_lock<std::mutex> lock(this->mtx_status_);
            hdr.SetFlags((this->flags_.to_ulong() << 3) | this->status_);
        }
        sock_.Transmit("SRP.EXE", 0, hdr.GetBuffor());
        AppLogger::Debug("id: "+ std::to_string(hdr.GetServiceID())
                    +" timestamp:"+std::to_string(hdr.GetTimestamp()));
        hdr.IncrementTimeStamp();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    });
}

void ExecController::SetStatus(Status status) {
    std::unique_lock<std::mutex> lock(this->mtx_status_);
    this->status_ = status;
}

ExecController::ExecController() {}

ExecController::~ExecController() {
}

void ExecController::ForceStop() {
    this->thread_.request_stop();
}


}  // namespace exec
}  // namespace diag
}  // namespace srp
