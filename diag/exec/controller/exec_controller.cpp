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


namespace simba{
namespace diag{
namespace exec{


ExecController::ExecController(u_int16_t service_id):
    service_id(service_id), status_(Status::Start_up) {
    this->sock_.Init(com::soc::SocketConfig{"SIMBA.DIAG." +
    std::to_string(this->service_id), 0, 0});
    this->thread_ = std::jthread(&ExecController::thread_func, this);
}
void ExecController::SetStatus(Status status) {
    this->status_ = status;
}

void ExecController::thread_func() {
    auto hdr = std::make_shared<simba::diag::exec::ExecHeader>(
        this->service_id, 0, this->status_);
    while (true) {
        std::vector<uint8_t> data = this->factory_.GetBuffer(hdr);
        hdr->IncrementTimeStamp();
        this->sock_.Transmit("SIMBA.DIAG."+std::to_string(this->service_id),
        0, data);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


}  // namepace exec
}  // namespace diag
}  // namespace simba