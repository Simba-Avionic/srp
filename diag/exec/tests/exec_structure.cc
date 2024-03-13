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

TEST(DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
  simba::diag::exec::ExecHeader hdr(123, 12, 1);
  EXPECT_EQ(hdr.GetServiceID(), 123);
  EXPECT_EQ(hdr.GetTimestamp(), 12);
  EXPECT_EQ(hdr.GetFlags(), 1);
}
