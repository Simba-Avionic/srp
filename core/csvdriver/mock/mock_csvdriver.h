/**
 * @file mock_csvdriver.h
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CORE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
#define CORE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
#include <string>
#include "gmock/gmock.h"
#include "core/csvdriver/Icsvdriver.h"
namespace simba {
namespace mock {
class MOCKCSVDRIVER : public logger::ICSVDriver {
 public:
  MOCK_METHOD(int, WriteLine, (const std::string& line), (override));
  MOCK_METHOD(int, Open, (const std::string& fileName, const std::string& HEADER), (override));
  MOCK_METHOD(void, Close, (), (override));
};
}   // namespace mock
}   // namespace simba

#endif  // CORE_CSVDRIVER_MOCK_MOCK_CSVDRIVER_H_
