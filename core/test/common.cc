#include <gtest/gtest.h>

#include <tuple>
#include <vector>

#include "common/endianess_converter.h"
#include "common/error_code.h"

class ErrorCodeToStringConversionTest
    : public testing::TestWithParam<
          std::tuple<simba::core::ErrorCode, std::string>> {};

TEST_P(ErrorCodeToStringConversionTest, ToStringConversion) {
  simba::core::ErrorCode error_code = std::get<0>(GetParam());
  std::string expected_string = std::get<1>(GetParam());
  EXPECT_EQ(std::to_string(error_code), expected_string);
}

INSTANTIATE_TEST_SUITE_P(
    ErrorCodes, ErrorCodeToStringConversionTest,
    testing::Values(std::make_tuple(simba::core::ErrorCode::kOk, "kOK"),
                    std::make_tuple(simba::core::ErrorCode::kNotDefine,
                                    "kNotDefine"),
                    std::make_tuple(simba::core::ErrorCode::kError, "kError"),
                    std::make_tuple(simba::core::ErrorCode::kConnectionError,
                                    "kConnectionError"),
                    std::make_tuple(simba::core::ErrorCode::kInitializeError,
                                    "kInitializeError"),
                    std::make_tuple(simba::core::ErrorCode::kBadVariableSize,
                                    "kBadVariableSize")))

TEST(EndianessConverter, testEndianessConverter) {
  simba::core::converter::EndianessConverter ec;
  std::vector<std::int8_t> data{1, 2, 5, 7, 9, 1, 23, 34, -23, -32, 2};
  for (std::int8_t d : data) {
    EXPECT_EQ(ec.Convert(d), d);
  }
}
