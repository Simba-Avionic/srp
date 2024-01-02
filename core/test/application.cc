#include <gtest/gtest.h>
#include "application/parm.h"




class ParmSplitTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {};

TEST_P(ParmSplitTest, BasicSplit) {
    std::string input = std::get<0>(GetParam());
    std::string expectedValue = std::get<1>(GetParam());
    std::string expectedName = std::get<2>(GetParam());

    simba::core::Parm parm{input};
    EXPECT_EQ(parm.GetValue(), expectedValue);
    EXPECT_EQ(parm.GetName(), expectedName);
}

INSTANTIATE_TEST_SUITE_P(
    ParmSplitTestCases, ParmSplitTest,
    testing::Values(
        std::make_tuple("abc=123", "123", "abc"),
        std::make_tuple("hello_world=444", "444", "hello_world"),
        std::make_tuple("32432", "", "32432"),
        std::make_tuple("abc==123", "=123", "abc"),
        std::make_tuple("abc", "", "abc"),
        std::make_tuple("", "", "")
    )
);
