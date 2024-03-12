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

class DiagDatFactoryTest : public testing::TestWithParam<std::tuple<int, int, int>> {
protected:

    void SetUp() override {
        std::tie(service_id, timestamp, flags) = GetParam();
        hdr = simba::diag::exec::ExecHeader(service_id, timestamp, flags);
    }

    int service_id;
    int timestamp;
    int flags;
    simba::diag::exec::ExecHeader hdr;
};

INSTANTIATE_TEST_SUITE_P(Default, DiagDatFactoryTest,
                        testing::Values(
                            std::make_tuple(123, 12, 0),
                            std::make_tuple(456, 34, 1),
                            std::make_tuple(789, 56, 2),
                            std::make_tuple(125, 22, 4),
                            std::make_tuple(426, 64, 8),
                            std::make_tuple(889, 46, 16),
                            std::make_tuple(323, 22, 32),
                            std::make_tuple(956, 84, 64),
                            std::make_tuple(149, 96, 133)
                        ));

TEST_P(DiagDatFactoryTest, CreateReadRequest) {
    simba::diag::exec::ExecMsgFactory sut;
    auto buf = sut.GetBuffer(std::make_shared<simba::diag::exec::ExecHeader>(hdr));
    auto hdr2 = sut.GetHeader(buf);
    ASSERT_EQ(hdr.GetBuffor(), buf);
    ASSERT_EQ(hdr.GetServiceID(), hdr2->GetServiceID());
    ASSERT_EQ(hdr.GetTimestamp(), hdr2->GetTimestamp());
}