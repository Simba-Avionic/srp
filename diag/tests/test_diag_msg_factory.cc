#include <gtest/gtest.h>

#include "diag/data/data_structure.h"
#include "diag/factories/diag_msg_factory.h"
TEST(DIAG_MSG_FACTORY, GetBuffer_without_payload) {
  const std::vector<uint8_t> expected{0x11, 0x11, 0x01, 0x00};

  simba::diag::data::DataStructure obj{0x1111, 0x01};

  const auto res = simba::diag::DiagMsgFactory::GetBuffer(obj);
  EXPECT_TRUE(res.HasValue());
  const auto sut = res.Value();
  EXPECT_EQ(expected.size(), sut.size());
  for (uint8_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], sut[i]);
  }
}

TEST(DIAG_MSG_FACTORY, GetBuffer_with_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> expected{0x11, 0x11, 0x01, 0x04,
                                      0x10, 0x11, 0x12, 0x13};

  simba::diag::data::DataStructure obj{0x1111, 0x01};
  obj.SetPayload(elements);
  const auto res = simba::diag::DiagMsgFactory::GetBuffer(obj);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();
  EXPECT_EQ(expected.size(), sut.size());
  for (uint8_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], sut[i]);
  }
}

TEST(DIAG_MSG_FACTORY, GetStructure_without_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> src{0x11, 0x11, 0x01, 0x00};

  simba::diag::data::DataStructure expected_obj{0x1111, 0x01};
  const auto res = simba::diag::DiagMsgFactory::GetStructure(src);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();

  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
}

TEST(DIAG_MSG_FACTORY, GetStructure_with_payload) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  const std::vector<uint8_t> src{0x11, 0x11, 0x01, 0x04,
                                 0x10, 0x11, 0x12, 0x13};

  simba::diag::data::DataStructure expected_obj{0x1111, 0x01};
  expected_obj.SetPayload(elements);
  const auto res = simba::diag::DiagMsgFactory::GetStructure(src);
  ASSERT_TRUE(res.HasValue());
  const auto sut = res.Value();

  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
  EXPECT_EQ(elements.size(), sut.GetPayload().size());
  for (uint8_t i = 0; i < sut.GetPayload().size(); i++) {
    EXPECT_EQ(expected_obj.GetPayload()[i], sut.GetPayload()[i]);
  }
}

TEST(DIAG_MSG_FACTORY, DoubleTest) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  simba::diag::data::DataStructure expected_obj{0x1111, 0x01};
  expected_obj.SetPayload(elements);
  const auto res = simba::diag::DiagMsgFactory::GetBuffer(expected_obj);
  ASSERT_TRUE(res.HasValue());
  const auto res2 = simba::diag::DiagMsgFactory::GetStructure(res.Value());
  ASSERT_TRUE(res2.HasValue());
  const auto sut = res2.Value();
  EXPECT_EQ(expected_obj.GetServiceID(), sut.GetServiceID());
  EXPECT_EQ(expected_obj.GetDiagID(), sut.GetDiagID());
  EXPECT_EQ(expected_obj.GetPayload().size(), sut.GetPayload().size());
  for (uint8_t i = 0; i < sut.GetPayload().size(); i++) {
    EXPECT_EQ(expected_obj.GetPayload()[i], sut.GetPayload()[i]);
  }
}
