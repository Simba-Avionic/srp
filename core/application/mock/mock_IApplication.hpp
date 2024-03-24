/**
 * @file mock_IApplication.hpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_APPLICATION_MOCK_MOCK_IAPPLICATION_HPP_
#define CORE_APPLICATION_MOCK_MOCK_IAPPLICATION_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include "gmock/gmock.h"

#include "core/application/Iapplication.h"

class MockApplication : public simba::core::IApplication {
 public:
  MOCK_METHOD(simba::core::ErrorCode, Run, (std::stop_token), (override));
  MOCK_METHOD(void, onRun, ((const std::unordered_map<std::string, std::string>&)), (override));
  MOCK_METHOD(void, StopApp, (), (override));
  MOCK_METHOD(void, RunApp, (int, const char*[]), (override));
  MOCK_METHOD(simba::core::ErrorCode, Initialize, ((const std::unordered_map<std::string, std::string>&)), (override));
};

#endif  // CORE_APPLICATION_MOCK_MOCK_IAPPLICATION_HPP_
