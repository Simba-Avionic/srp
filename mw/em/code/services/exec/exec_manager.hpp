/**
 * @file exec_manager.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MW_EM_CODE_SERVICES_EXEC_EXEC_MANAGER_HPP_
#define MW_EM_CODE_SERVICES_EXEC_EXEC_MANAGER_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>  // NOLINT
#include <bitset>
#include <memory>
#include <utility>
#include <queue>
#include <optional>

#include "communication-core/sockets/ipc_socket.h"
#include "diag/exec/controller/exec_controller.hpp"
#include "mw/em/code/services/em/app_config.h"
#include "diag/exec/data/exec_header.hpp"
#include "core/logger/Logger.h"

namespace simba {
namespace mw {
namespace exec {

enum ExecState {
    kRunning,
    kRestartNeeded,
};


struct Service{
    /**
     * @brief numer ostatniego timestamp
     */
    uint16_t last_timestamp{0};
    /**
     * @brief czas otrzymania ostatniego hb
     * 
     */
    std::chrono::_V2::system_clock::time_point last_timestamp_time{};
    /**
     * @brief liczba hb otrzymanych w niepoprawnym czasie
     * 
     */
    uint8_t invalid_timestamp_num{0};
    /**
     * @brief status aplikacji
     * 
     */
    diag::exec::Status state{diag::exec::Status::Start_up};
    /**
     * @brief dodatkowe flagi
     * 
     */
    std::bitset<5> flags{0};

    /**
     * @brief Stan aplikacji według em application
     * 
     */
    ExecState execState{ExecState::kRunning};


    Service() {}
    explicit Service(uint16_t timestamp) {
        this->last_timestamp = timestamp;
        this->last_timestamp_time = std::chrono::high_resolution_clock::now();
    }
    Service(uint16_t timestamp, diag::exec::Status status) {
        this->last_timestamp = timestamp;
        this->last_timestamp_time = std::chrono::high_resolution_clock::now();
        this->state = status;
    }
    Service(uint16_t timestamp, diag::exec::Status status, std::bitset<5> flags) {
        this->last_timestamp = timestamp;
        this->last_timestamp_time = std::chrono::high_resolution_clock::now();
        this->state = status;
        this->flags = flags;
    }
};

class ExecManager{
 private:
  com::soc::IpcSocket sock_;
  std::mutex mtx;
  std::unordered_map<uint16_t, Service> db_;
  uint16_t my_service_id;
  void RxCallback(const std::string& ip, const std::uint16_t& port,
                        std::vector<std::uint8_t> payload);
  std::pair<simba::diag::exec::Status, std::bitset<5>> getStatusAndFlags(uint8_t data);
 public:
  std::queue<uint16_t> CheckAppCondition();
  void SetApps(std::vector<simba::em::service::data::AppConfig> apps);
  void Init(uint16_t service_id);
  void RestartedApp(uint16_t appID);
};

}  // namespace exec
}  // namespace mw
}  // namespace simba

#endif  // MW_EM_CODE_SERVICES_EXEC_EXEC_MANAGER_HPP_
