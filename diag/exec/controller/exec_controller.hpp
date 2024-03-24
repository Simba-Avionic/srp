/**
 * @file exec_controller.hpp
 * @author Wiktor Kawka (asinesak353@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-28
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#ifndef DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_
#define DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_

#include <vector>
#include <memory>
#include <chrono> // NOLINT
#include <thread> // NOLINT
#include <mutex> // NOLINT
#include <bitset>

#include "status.hpp"

namespace simba {
namespace diag {
namespace exec {

class ExecController {
 private:
    uint16_t service_id;
    std::jthread thread_;
    std::mutex mtx_status_;
    Status status_;
    std::bitset<5> flags_;

 public:
    void Init(uint16_t service_id);
    ExecController();
    void SetStatus(Status status);
    void SetFlags(std::bitset<5> flags);
    ~ExecController();
};
}  // namespace exec
}  // namespace diag
}  // namespace simba

#endif  // DIAG_EXEC_CONTROLLER_EXEC_CONTROLLER_HPP_
