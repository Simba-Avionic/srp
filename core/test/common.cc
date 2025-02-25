/**
 * @file common.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include <tuple>
#include <vector>

#include "core/common/endianess_converter.h"
#include "core/common/error_code.h"

class ErrorCodeToStringConversionTest
    : public testing::TestWithParam<
          std::tuple<srp::core::ErrorCode, std::string>> {};

TEST_P(ErrorCodeToStringConversionTest, ToStringConversion) {
  srp::core::ErrorCode error_code = std::get<0>(GetParam());
  std::string expected_string = std::get<1>(GetParam());
  EXPECT_EQ(std::to_string(error_code), expected_string);
}

INSTANTIATE_TEST_SUITE_P(
    ErrorCodes, ErrorCodeToStringConversionTest,
    testing::Values(std::make_tuple(srp::core::ErrorCode::kOk, "kOK"),
                    std::make_tuple(srp::core::ErrorCode::kNotDefine,
                                    "kNotDefine"),
                    std::make_tuple(srp::core::ErrorCode::kError, "kError"),
                    std::make_tuple(srp::core::ErrorCode::kConnectionError,
                                    "kConnectionError"),
                    std::make_tuple(srp::core::ErrorCode::kInitializeError,
                                    "kInitializeError"),
                    std::make_tuple(srp::core::ErrorCode::kBadVariableSize,
                                    "kBadVariableSize")));

TEST(EndianessConverter, testEndianessConverter) {
  srp::core::converter::EndianessConverter ec;
  std::vector<std::int8_t> data{1, 2, 5, 7, 9, 1, 23, 34, -23, -32, 2};
  for (std::int8_t d : data) {
    EXPECT_EQ(ec.Convert(d), d);
  }
}
