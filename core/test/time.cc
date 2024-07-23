/**
 * @file time.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "core/time/sys_time.hpp"

class TestWrapper : public simba::core::time::TimeChanger {
 public:
  std::string TestConvertVariableToCommand(simba::core::time::Date_t date) {
    return ConvertVariableToCommand(date);
  }
  std::optional<simba::core::time::Date_t> TestparseDate(const std::string& dateStr) {
    return this->ParseDate(dateStr);
  }
};


class ParseDateTest : public
        ::testing::TestWithParam<std::pair<std::string, simba::core::time::Date_t>> {
};

TEST_P(ParseDateTest, ParseDate) {
    TestWrapper wrapper;
    const auto& param = GetParam();
    std::optional<simba::core::time::Date_t> date = param.second;
    auto result = wrapper.TestparseDate(param.first);

    // Compare individual components of the parsed date
    EXPECT_EQ(result.value().day, date.value().day);
    EXPECT_EQ(result.value().hour, date.value().hour);
    EXPECT_EQ(result.value().minute, date.value().minute);
    EXPECT_EQ(result.value().month, date.value().month);
    EXPECT_EQ(result.value().seconds, date.value().seconds);
    EXPECT_EQ(result.value().year, date.value().year);
}

INSTANTIATE_TEST_SUITE_P(
    ParseDateTests,
    ParseDateTest,
    ::testing::Values(
        std::make_pair("Sun Jun  2 21:40:23 UTC 2024",
                        simba::core::time::Date_t{simba::core::time::Date_t{2024, 6, 2, 21, 40, 23}})
    )
);
class ConvertVariableToCommandTest : public
                    ::testing::TestWithParam<std::pair<simba::core::time::Date_t, std::string>> {
};

TEST_P(ConvertVariableToCommandTest, ConvertVariableToCommand) {
    TestWrapper wrapper;
    const auto& param = GetParam();
    EXPECT_EQ(wrapper.TestConvertVariableToCommand(param.first), param.second);
}

INSTANTIATE_TEST_SUITE_P(
    ConvertVariableToCommandTests,
    ConvertVariableToCommandTest,
    ::testing::Values(
        std::make_pair(simba::core::time::Date_t{2024, 6, 2, 21, 40, 23}, "date 060221402024.23"),
        std::make_pair(simba::core::time::Date_t{2020, 1, 3, 28, 41, 55}, "date 010328412020.55")
    )
);
