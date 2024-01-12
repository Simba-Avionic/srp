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

#include "diag/base/data/data_structure.h"
#include "diag/base/factories/diag_data_factory.h"

TEST(DIAG_DAT_FACTORY, CreateReadRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateReadRequest(0x01, 0x01, 0x01,
                                                             0x01, {});
  ASSERT_TRUE(sut.has_value());
  ASSERT_EQ(sut.value().GetDiagID() & 0b11, 0x01);
}
TEST(DIAG_DAT_FACTORY, CreateWriteRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateWriteRequest(0x01, 0x01, 0x01,
                                                              0x01, {});
  ASSERT_TRUE(sut.has_value());
  ASSERT_EQ(sut.value().GetDiagID() & 0b11, 0x00);
}
TEST(DIAG_DAT_FACTORY, CreateResponse) {
  auto sut =
      simba::diag::DiagDataFactory::CreateResponse(0x01, 0x01, 0x01, 0x01, {});
  ASSERT_TRUE(sut.has_value());
  ASSERT_EQ(sut.value().GetDiagID() & 0b11, 0x03);
}

TEST(DIAG_DAT_FACTORY, CreateJobRequest) {
  auto sut = simba::diag::DiagDataFactory::CreateJobRequest(0x01, 0x01, 0x01,
                                                            0x01, {});
  ASSERT_TRUE(sut.has_value());
  ASSERT_EQ(sut.value().GetDiagID() & 0b11, 0x02);
}
