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
#include "exec_controller.hpp"
#include "core/logger/Logger.h"
#include "communication-core/sockets/ipc_socket.h"
#include "diag/exec/factories/exec_msg_factory.hpp"

namespace simba {
namespace diag {
namespace exec {

void ExecController::Init(uint16_t service_id) {
    this->mtx_status_.lock();
    this->status_ = Status::Start_up;
    this->flags_ = std::bitset<5>{"00000"};
    this->thread_ = std::jthread([&](std::stop_token stop_token) {
    auto factory_ = ExecMsgFactory();
    auto sock_ = com::soc::IpcSocket();
    auto hdr = ExecHeader(service_id, 1, this->flags_.to_ulong() << 3 | this->status_);
    this->mtx_status_.unlock();
    while (!stop_token.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->mtx_status_.lock();
        hdr.SetFlags(this->flags_.to_ulong() << 3 | this->status_);
        this->mtx_status_.unlock();
        auto data = factory_.GetBuffer(std::make_shared<ExecHeader>(hdr));
        sock_.Transmit("SIMBA.EXE", 0, data);
        AppLogger::Info("id: "+ std::to_string(hdr.GetServiceID())
                    +" timestamp:"+std::to_string(hdr.GetTimestamp()));
        hdr.IncrementTimeStamp();
    }
    });
}

void ExecController::SetStatus(Status status) {
    this->mtx_status_.lock();
    this->status_ = status;
    this->mtx_status_.unlock();
}

ExecController::ExecController() {}

ExecController::~ExecController() {
}


}  // namespace exec
}  // namespace diag
}  // namespace simba
