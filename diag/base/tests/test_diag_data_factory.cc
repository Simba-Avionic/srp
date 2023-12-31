#include <gtest/gtest.h>

#include "diag/base/data/data_structure.h"
#include "diag/base/factories/diag_data_factory.h"

TEST(DIAG_DAT_FACTORY, CreateReadRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateReadRequest(0x01, 0x01, 0x01,
                                                             0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b11, 0x01);
}
TEST(DIAG_DAT_FACTORY, CreateWriteRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateWriteRequest(0x01, 0x01, 0x01,
                                                              0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b11, 0x00);
}
TEST(DIAG_DAT_FACTORY, CreateResponse) {
  auto sut =
      simba::diag::DiagDataFactory::CreateResponse(0x01, 0x01, 0x01, 0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b11, 0x03);
}

TEST(DIAG_DAT_FACTORY, CreateJobRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateJobRequest(0x01, 0x01, 0x01,
                                                            0x01, {});
  ASSERT_TRUE(sut.HasValue());
  ASSERT_EQ(sut.Value().GetDiagID() & 0b11, 0x02);
}
