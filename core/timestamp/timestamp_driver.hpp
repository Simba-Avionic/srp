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
#include <thread>  // NOLINT
#include "bindings/common/shm/shm_skeleton.h"
#include "bindings/common/shm/shm_proxy.h"
namespace srp {
namespace core {
namespace timestamp {

class TimestampController {
 private:
  const ara::core::InstanceSpecifier instance_;
  bindings::com::shm::ShmProxy<int64_t> proxy_;
 public:
  std::optional<int64_t> GetNewTimeStamp();
  int64_t GetDeltaTime(const int64_t now, const int64_t previous);
  void Init();
  TimestampController();
};


class TimestampMaster {
 private:
  const ara::core::InstanceSpecifier instance_;
  bindings::com::shm::ShmSkeleton<int64_t> skeleton_;
  int64_t start;
 public:
  TimestampMaster();
  int64_t GetNewTimeStamp();
  void CorrectStartPoint(const int64_t offset);
  void Init();
};

}  // namespace timestamp
}  // namespace core
}  // namespace srp

#endif  // CORE_TIMESTAMP_TIMESTAMP_DRIVER_HPP_
