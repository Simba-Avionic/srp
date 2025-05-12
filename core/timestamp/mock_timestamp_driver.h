/**
 * @file mock_timestamp_driver.h
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-04-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CORE_TIMESTAMP_MOCK_TIMESTAMP_DRIVER_H_
#define CORE_TIMESTAMP_MOCK_TIMESTAMP_DRIVER_H_

#include <gmock/gmock.h>
#include "core/timestamp/Itimestamp_driver.h"

class MockTimestampController : public srp::core::timestamp::ITimestampController {
 public:
  MOCK_METHOD(std::optional<int64_t>, GetNewTimeStamp, (), (override));
  MOCK_METHOD(bool, Init, (), (override));
};

class MockTimestampMaster : public srp::core::timestamp::ITimestampMaster {
 public:
  MOCK_METHOD(int64_t, GetNewTimeStamp, (), (override));
  MOCK_METHOD(void, CorrectStartPoint, (const int64_t offset), (override));
  MOCK_METHOD(bool, Init, (), (override));
};

#endif  // CORE_TIMESTAMP_MOCK_TIMESTAMP_DRIVER_H_
