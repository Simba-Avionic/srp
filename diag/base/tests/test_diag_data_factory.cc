#include <gtest/gtest.h>

#include "diag/base/data/data_structure.h"
#include "diag/base/factories/diag_data_factory.h"

TEST(DIAG_DAT_FACTORY, Creat_request) {
  auto sut =
      simba::diag::DiagDataFactory::CreatRequest(0x01, 0x01, 0x01, 0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b1111, 0x00);
}
TEST(DIAG_DAT_FACTORY, Creat_requestNoResponse) {
  auto sut = simba::diag::DiagDataFactory::CreatRequestNoResponse(
      0x01, 0x01, 0x01, 0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b1111, 0x02);
}
TEST(DIAG_DAT_FACTORY, Creat_Response) {
  auto sut =
      simba::diag::DiagDataFactory::CreatResponse(0x01, 0x01, 0x01, 0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b1111, 0x01);
}