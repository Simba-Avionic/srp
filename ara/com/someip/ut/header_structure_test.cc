/**
 * @file header_structure_test.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include <bit>  // NOLINT
#include <iostream>

#include "ara/com/someip/HeaderStructure.h"

TEST(HEADERTEST, Conv_Test) {
  ara::com::someip::HeaderStructure sut{};
  sut.service_id = 0x0102;
  sut.method_id = 0x0304;
  sut.length = 0x05060708;
  sut.request_id = 0x0910;
  sut.session_id = 0x1112;
  sut.protocol_version = 0x13;
  sut.interface_version = 0x14;
  sut.message_type = 0x15;
  const auto vec =
      srp::data::Convert2Vector<ara::com::someip::HeaderStructure>::Conv(sut);
  EXPECT_EQ(vec.at(1), 0x02);
  EXPECT_EQ(vec.at(0), 0x01);
  EXPECT_EQ(vec.size(), 4 * 4);
  const auto new_sut_r =
      srp::data::Convert<ara::com::someip::HeaderStructure>::Conv(vec);
  ASSERT_TRUE(new_sut_r.HasValue());
  const auto new_sut = new_sut_r.Value();
  EXPECT_EQ(sut.service_id, new_sut.service_id);
  EXPECT_EQ(sut.method_id, new_sut.method_id);
  EXPECT_EQ(sut.length, new_sut.length);
  EXPECT_EQ(sut.request_id, new_sut.request_id);
  EXPECT_EQ(sut.session_id, new_sut.session_id);
  EXPECT_EQ(sut.protocol_version, new_sut.protocol_version);
  EXPECT_EQ(sut.interface_version, new_sut.interface_version);
  EXPECT_EQ(sut.message_type, new_sut.message_type);
}
