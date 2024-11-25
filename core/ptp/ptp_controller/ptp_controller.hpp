/**
 * @file ptp_controller.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_PTP_PTP_CONTROLLER_PTP_CONTROLLER_HPP_
#define CORE_PTP_PTP_CONTROLLER_PTP_CONTROLLER_HPP_

#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include "ara/com/shm/shm_skeleton.h"
using kTime_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

namespace simba {
namespace ptp {

class PTPController {
 private:
  ara::com::shm::ShmSkeleton<kTime_t> shm_;
 public:
  PTPController();
  int Init();
  ~PTPController();
};

}  // namespace ptp
}  // namespace simba

#endif  // CORE_PTP_PTP_CONTROLLER_PTP_CONTROLLER_HPP_
