/**
 * @file global_timestamp.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_PTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_
#define CORE_PTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_

#include <chrono>  // NOLINT
#include <optional>
#include "ara/com/shm/shm_proxy.h"
using kTime_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

namespace simba {
namespace ptp {

class GlobalTimestampController {
 private:
  ara::com::shm::ShmProxy<kTime_t> shm_;
 public:
  GlobalTimestampController();
  ~GlobalTimestampController();
  /**
   * @brief Get the Global Timestamp object
   * 
   * @return std::optional<uint64_t> 
   */
  std::optional<uint64_t> GetTimestamp();
};

}  // namespace ptp
}  // namespace simba

#endif  // CORE_PTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_
