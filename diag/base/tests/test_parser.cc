/**
 * @file test_parser.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "diag/base/data/data_structure.h"
#include "diag/base/data/parser.h"
TEST(PARSER, GetBuffer_without_payload) {
  const std::vector<uint8_t> expected{0x11, 0x11, 0x00, 0x02,
                                      0x01, 0x00, 0x00, 0x00};

  simba::diag::data::DataStructure obj{0x1111, 0x01, 0x02, 0x00};

  const auto res = simba::diag::Parser::GetBuffer(obj);
  EXPECT_TRUE(res.HasValue());
  const auto sut = res.Value();
  EXPECT_EQ(expected.size(), sut.size());
  for (uint8_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], sut[i]);
  }
}

TEST(PARSER, GetBuffer_with_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> expected{0x11, 0x11, 0x00, 0x02, 0x01, 0x00,
                                      0x00, 0x04, 0x10, 0x11, 0x12, 0x13};

  simba::diag::data::DataStructure obj{0x1111, 0x01, 0x02, 0x00};
  obj.SetPayload(elements);
  const auto res = simba::diag::Parser::GetBuffer(obj);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();
  EXPECT_EQ(expected.size(), sut.size());
  for (uint8_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], sut[i]);
  }
}

TEST(PARSER, GetStructure_without_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> src{0x11, 0x11, 0x00, 0x02,
                                 0x01, 0x00, 0x00, 0x00};

  simba::diag::data::DataStructure expected_obj{0x1111, 0x01, 0x02, 0x00};
  const auto res = simba::diag::Parser::GetStructure(src);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();

  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
}

TEST(PARSER, GetStructure_with_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> src{0x11, 0x11, 0x00, 0x02, 0x01, 0x00,
                                 0x00, 0x04, 0x10, 0x11, 0x12, 0x13};

  simba::diag::data::DataStructure expected_obj{0x1111, 0x01, 0x02, 0x00};
  expected_obj.SetPayload(elements);
  const auto res = simba::diag::Parser::GetStructure(src);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();

  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
  EXPECT_EQ(elements.size(), sut.GetPayload().size());
  for (uint8_t i = 0; i < sut.GetPayload().size(); i++) {
    EXPECT_EQ(expected_obj.GetPayload()[i], sut.GetPayload()[i]);
  }
}

TEST(PARSER, DoubleTest) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  simba::diag::data::DataStructure expected_obj{0x1111, 0x01, 0x02, 0x00};
  expected_obj.SetPayload(elements);
  const auto res = simba::diag::Parser::GetBuffer(expected_obj);
  ASSERT_TRUE(res.HasValue());
  const auto res2 = simba::diag::Parser::GetStructure(res.Value());
  ASSERT_TRUE(res2.HasValue());
  const auto sut = res2.Value();
  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
  EXPECT_EQ(expected_obj.GetPayload().size(), sut.GetPayload().size());
  for (uint8_t i = 0; i < sut.GetPayload().size(); i++) {
    EXPECT_EQ(expected_obj.GetPayload()[i], sut.GetPayload()[i]);
  }
}
