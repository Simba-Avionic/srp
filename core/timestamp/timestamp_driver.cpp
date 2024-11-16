/**
 * @file timestamp_driver.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "core/timestamp/timestamp_driver.hpp"

namespace simba {
namespace core {
namespace timestamp {

void TimestampController::Start() {
  start = std::chrono::high_resolution_clock::now();
}
uint64_t TimestampController::GetNewTimeStamp() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return duration.count();
}

}  // namespace timestamp
}  // namespace core
}  // namespace simba
