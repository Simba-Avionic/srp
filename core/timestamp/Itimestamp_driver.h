/**
 * @file Itimestamp_driver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CORE_TIMESTAMP_ITIMESTAMP_DRIVER_H_
#define CORE_TIMESTAMP_ITIMESTAMP_DRIVER_H_

#include <optional>
#include <cstdint>

namespace srp {
namespace core {
namespace timestamp {

class ITimestampController {
 public:
  virtual std::optional<int64_t> GetNewTimeStamp() = 0;
  virtual int64_t GetDeltaTime(const int64_t now, const int64_t previous) = 0;
  virtual bool Init() = 0;
  virtual ~ITimestampController() = default;
};

class ITimestampMaster {
 public:
  virtual int64_t GetNewTimeStamp() = 0;
  virtual void CorrectStartPoint(const int64_t offset) = 0;
  virtual bool Init() = 0;
  virtual ~ITimestampMaster() = default;
};

}  // namespace timestamp
}  // namespace core
}  // namespace srp
#endif  // CORE_TIMESTAMP_ITIMESTAMP_DRIVER_H_
