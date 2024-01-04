#include <gtest/gtest.h>

#include "diag/dtc/factories/dtc_msg_factory.hpp"
#include "diag/dtc/data/dtc_header.hpp"

TEST(DATA_FACTORIES,DATA_FACTORIES_TEST){
    simba::diag::dtc::DtcMsgFactory factory;
    std::vector<uint8_t> payload={0x0,0x1,0x2,0x3,0x4,0x5};
    uint16_t id=0x0001;
    uint8_t status=0x03;
    simba::diag::dtc::DtcHeader hdr{id,status};
    std::vector<uint8_t> data=factory.GetBuffer(std::make_shared<simba::diag::dtc::DtcHeader>(hdr),payload);
    auto hdr2=factory.GetHeader(data);
    EXPECT_EQ(hdr2->GetDtcID(),hdr.GetDtcID());
    EXPECT_EQ(hdr2->GetDtcStatus(),hdr.GetDtcStatus());
    EXPECT_EQ(hdr2->GetLength(),hdr.GetLength());
}