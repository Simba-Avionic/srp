/**
 * @file test_subscribe_msg.cpp
 * @author Jacek Kukawski (jacekka6@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2024-05-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "mw/temp/subscribe_msg/subscribe_header.h"

class SUBSCRIBE_HEADER : public ::testing::TestWithParam<uint16_t> {
 protected:
  uint16_t id;
  simba::mw::temp::SubscribeHeader hdr;

  void SetUp() override {
    id = GetParam();
  }
};

INSTANTIATE_TEST_SUITE_P(SUBSCRIBE_HEADER_PARAMS,
                         SUBSCRIBE_HEADER,
                         ::testing::Values(0x0010));

TEST_P(SUBSCRIBE_HEADER, CONSTRUCTOR_CHECK) {
    hdr = simba::mw::temp::SubscribeHeader{id};
    EXPECT_EQ(id, hdr.GetServiceID());
}
