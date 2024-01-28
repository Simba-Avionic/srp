/**
 * @file exec_controller.hpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
*/

#ifndef DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_
#define DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_

#include <vector>
#include <memory>
#include <thread>

#include "communication-core/sockets/ipc_socket.h"
#include "diag/exec/factories/exec_msg_factory.hpp"

namespace simba {
namespace diag {
namespace exec {

enum Status {
    Start_up = 0x00,
    Running = 0x01,
    Running_with_dtc = 0x02,
    Running_after_dtc = 0x03,
    Error = 0x04,
    Running_after_error = 0x05,
    Running_ignore_DTC = 0x06,
    Running_ignore_error = 0x07
};

class ExecController {
    private:
        const u_int16_t service_id;
        const u_int16_t init_time;
        u_int16_t time;
        static ExecMsgFactory factory_;
        com::soc::IpcSocket sock_;
        std::thread thread_;
        Status status_;
        void Run();

    public:
        ExecController(u_int16_t service_id, u_int16_t init_time);
        void SetStatus(Status status);
        ~ExecController();

};
}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_
