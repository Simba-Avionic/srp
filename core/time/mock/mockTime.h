/**
 * @file mockTime.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_TIME_MOCK_MOCKTIME_H_
#define CORE_TIME_MOCK_MOCKTIME_H_

#include <string>
#include <vector>
#include "gmock/gmock.h"

#include "core/time/sys_time.hpp"
#include "core/common/error_code.h"

class MockTime : public srp::core::time::TimeChanger {
 public:
  MOCK_METHOD(core::ErrorCode, ChangeSystemTime, (std::string time));
  MOCK_METHOD(core::ErrorCode, ChangeSystemTime, (srp::core::time::Date_t date));
  MOCK_METHOD(std::optional<srp::core::time::Date_t>, ReadSystemTime, ());
};


#endif  // CORE_TIME_MOCK_MOCKTIME_H_
