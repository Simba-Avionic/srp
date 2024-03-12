/**
 * @file test_data_structure.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "diag/exec/data/exec_header.hpp"

class DataStructureTest : public testing::TestWithParam<std::tuple<int, int, int>> {
 protected:
  void SetUp() override {
    std::tie(service_id, timestamp, flags) = GetParam();
    header = simba::diag::exec::ExecHeader(service_id, timestamp, flags);
  }
  int service_id;
  int timestamp;
  int flags;
  simba::diag::exec::ExecHeader header;
};

INSTANTIATE_TEST_SUITE_P(Default, DataStructureTest,
                        testing::Values(
                            std::make_tuple(123, 12, 0),
                            std::make_tuple(456, 34, 1),
                            std::make_tuple(789, 56, 2),
                            std::make_tuple(125, 22, 4),
                            std::make_tuple(426, 64, 8),
                            std::make_tuple(889, 46, 16),
                            std::make_tuple(323, 22, 32),
                            std::make_tuple(956, 84, 64),
                            std::make_tuple(149, 96, 133)
));

TEST_P(DataStructureTest, ConstructorCheck) {
    EXPECT_EQ(header.GetServiceID(), service_id);
    EXPECT_EQ(header.GetTimestamp(), timestamp);
    EXPECT_EQ(header.GetFlags(), flags);
}
