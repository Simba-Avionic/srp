/**
 * @file test_diag_data_factory.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "diag/exec/factories/exec_msg_factory.hpp"

TEST(DIAG_DAT_FACTORY, CreateReadRequest) {
  auto hdr = simba::diag::exec::ExecHeader(123, 12, 2);
  auto sut = simba::diag::exec::ExecMsgFactory();
  auto buf = sut.GetBuffer(std::make_shared<simba::diag::exec::ExecHeader>(hdr));
  auto hdr2 = sut.GetHeader(buf);
  ASSERT_EQ(hdr.GetBuffor(), buf);
  ASSERT_EQ(hdr.GetServiceID(), hdr2->GetServiceID());
  ASSERT_EQ(hdr.GetTimestamp(), hdr2->GetTimestamp());
}
