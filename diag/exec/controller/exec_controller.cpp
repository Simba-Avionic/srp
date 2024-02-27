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


namespace simba {
namespace diag {
namespace exec {


ExecController::ExecController(u_int16_t service_id):
    service_id(service_id), status_(Status::Start_up) {
    this->sock_.Init(com::soc::SocketConfig{"SIMBA.DIAG." +
    std::to_string(this->service_id), 0, 0});
    this->thread_ = std::jthread(&ExecController::thread_func, this, std::placeholders::_1);
}
void ExecController::SetStatus(Status status) {
    this->status_ = status;
}

void ExecController::thread_func(std::stop_token token) {
    auto hdr = std::make_shared<simba::diag::exec::ExecHeader>(
        this->service_id, 0, this->status_);
    while (!token.stop_requested()) {
        std::vector<uint8_t> data = this->factory_.GetBuffer(hdr);
        hdr->IncrementTimeStamp();
        this->sock_.Transmit("SIMBA.EXE", 0, data);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

ExecController::~ExecController() {
    this->thread_.request_stop();
}


}  // namespace exec
}  // namespace diag
}  // namespace simba
