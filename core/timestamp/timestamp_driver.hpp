/**
 * @file timestamp_driver.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CORE_TIMESTAMP_TIMESTAMP_DRIVER_HPP_
#define CORE_TIMESTAMP_TIMESTAMP_DRIVER_HPP_

#include <chrono>  // NOLINT

namespace srp {
namespace core {
namespace timestamp {

class TimestampController {
 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
 public:
  void Start();
  uint64_t GetNewTimeStamp();
};

}  // namespace timestamp
}  // namespace core
}  // namespace srp

#endif  // CORE_TIMESTAMP_TIMESTAMP_DRIVER_HPP_
