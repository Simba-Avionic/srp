#include <gtest/gtest.h>

#include "diag/base/data/data_structure.h"

TEST(DATA_STRUCTURE, CONSTRUCTOR_CHECK) {
  const std::vector<uint8_t> elements{0x10, 0x11, 0x12, 0x13};
  simba::diag::data::DataStructure sut{0x1111, 0x01,0x02};
  EXPECT_EQ(0x1111, sut.GetServiceID());
  EXPECT_EQ(0x01, sut.GetDiagID());
  sut.SetPayload(elements);
  EXPECT_EQ(0x1111, sut.GetServiceID());
  EXPECT_EQ(0x01, sut.GetDiagID());
  EXPECT_EQ(0x1111, sut.GetServiceID());
  EXPECT_EQ(0x02, sut.GetSenderID());
  EXPECT_EQ(elements.size(), sut.GetPayload().size());
  for (uint8_t i = 0; i < sut.GetPayload().size(); i++) {
    EXPECT_EQ(elements[i], sut.GetPayload()[i]);
  }
}
