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
#ifndef CORE_NTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_
#define CORE_NTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_

#include <chrono>  // NOLINT
#include <optional>
#include "ara/com/shm/shm_proxy.h"
using kTime_t = uint64_t;

namespace simba {
namespace ntp {

static uint64_t GetNowInMS() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

class GlobalTimestampController {
 private:
  ara::com::shm::ShmProxy<kTime_t> shm_;
  uint64_t globalTimestamp_;
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

}  // namespace ntp
}  // namespace simba

#endif  // CORE_NTP_TIMESTAMP_GLOBAL_TIMESTAMP_HPP_
