/**
 * @file test_routable_uds_service.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-11-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "bindings/common/socket/mock/socket_stream_mock.h"
#include "ara/core/model_db.h"
#include "ara/diag/routing/routable_uds_service.h"
#include "ara/diag/uds_error_domain.h"
namespace ara {
namespace diag {

class RoutableUdsServiceFixture : public ::testing::Test {
 protected:
  std::shared_ptr<ara::com::soc::SocketStreamMock> socket_mock_;

 public:
  RoutableUdsServiceFixture(/* args */) {
    socket_mock_ = std::make_shared<ara::com::soc::SocketStreamMock>();
    auto& db_ = ara::core::ModelDataBase::GetInstance();
    {
      const auto res = ara::core::model::ModelUds::CreatItem(
          34U, 4384U, "2232608", ara::core::model::ModelUds::Direction::kOut);
      std::ignore = db_.AddNewItem("/srp/test/example", {res.Value()});
      const auto res2 =
          db_.ResolveInstanceSpecifier<ara::core::model::ModelUds>(
              ara::core::InstanceSpecifier{"/srp/test/example"});
      EXPECT_TRUE(res2.HasValue());
    }
  }
};

class TestRoutableUdsService : public routing::RoutableUdsService {
 public:
  TestRoutableUdsService(const ara::core::InstanceSpecifier& specifier,
                         std::unique_ptr<ara::com::soc::ISocketStream> sock)
      : routing::RoutableUdsService(specifier, 34U, std::move(sock)) {}

  MOCK_METHOD(ara::core::Result<OperationOutput>, TestMethod,
              (const std::vector<uint8_t>&));
  virtual ara::core::Result<OperationOutput> HandleMessage(
      const std::vector<uint8_t>& requestData) {
    return this->TestMethod(requestData);
  }

  ~TestRoutableUdsService() = default;
};

TEST_F(RoutableUdsServiceFixture,
       GivenInvalidModelPathWhenOfferExpectNoCallAndError) {
  auto sut = TestRoutableUdsService(
      ara::core::InstanceSpecifier{"/srp/test/other"},
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));

  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(0);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_)).Times(0);
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(0);

  const auto res = sut.Offer();

  EXPECT_FALSE(res.HasValue());
  EXPECT_FALSE(sut.IsOffered());
}

TEST_F(RoutableUdsServiceFixture,
       GivenInvalidModelPathWhenStopOfferExpectNoCall) {
  auto sut = TestRoutableUdsService(
      ara::core::InstanceSpecifier{"/srp/test/other"},
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));

  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(0);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_)).Times(0);
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(0);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(0);

  EXPECT_EQ(34U, sut.GetSid());

  const auto res = sut.Offer();

  EXPECT_FALSE(res.HasValue());
  EXPECT_FALSE(sut.IsOffered());

  sut.StopOffer();
}

TEST_F(RoutableUdsServiceFixture, GivenValidModelPathWhenOfferExpectOnceCall) {
  const auto instance = ara::core::InstanceSpecifier{"/srp/test/example"};
  auto sut = TestRoutableUdsService(
      instance,
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));

  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(1);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(0);

  const auto res = sut.Offer();

  EXPECT_TRUE(res.HasValue());
  EXPECT_TRUE(sut.IsOffered());
}

TEST_F(RoutableUdsServiceFixture,
       GivenValidModelPathWhenMultipleOfferExpectOnceCall) {
  const auto instance = ara::core::InstanceSpecifier{"/srp/test/example"};
  auto sut = TestRoutableUdsService(
      instance,
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));

  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(1);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(0);
  {
    const auto res = sut.Offer();
    EXPECT_TRUE(res.HasValue());
  }
  {
    const auto res = sut.Offer();
    EXPECT_FALSE(res.HasValue());
  }
  {
    const auto res = sut.Offer();
    EXPECT_FALSE(res.HasValue());
  }
  EXPECT_TRUE(sut.IsOffered());
}

TEST_F(RoutableUdsServiceFixture,
       GivenValidModelPathWhenStopOfferExpectOnceCall) {
  const auto instance = ara::core::InstanceSpecifier{"/srp/test/example"};
  auto sut = TestRoutableUdsService(
      instance,
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));

  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(1);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(1);

  const auto res = sut.Offer();

  EXPECT_TRUE(res.HasValue());
  EXPECT_TRUE(sut.IsOffered());

  sut.StopOffer();

  EXPECT_FALSE(sut.IsOffered());
}

TEST_F(RoutableUdsServiceFixture,
       GivenValidModelPathWhenRequestReceivedExpectPositiveResponse) {
  const auto instance = ara::core::InstanceSpecifier{"/srp/test/example"};
  auto sut = TestRoutableUdsService(
      instance,
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));
  ara::com::soc::RXCallbackStream callback_;
  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_))
      .WillOnce(::testing::Invoke(
          [&callback_](ara::com::soc::RXCallbackStream&& callback) {
            callback_ = std::move(callback);
          }));
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(1);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(0);

  EXPECT_CALL(sut, TestMethod(::testing::_))
      .WillOnce(::testing::Return(OperationOutput{{1, 2, 3, 4, 56, 7}}));
  const auto res = sut.Offer();
  EXPECT_TRUE(res.HasValue());
  EXPECT_TRUE(sut.IsOffered());

  const auto call_res = callback_({0x22, 0x11, 0x20});
  const std::vector<uint8_t> expect_val{1, 2, 3, 4, 56, 7};
  EXPECT_EQ(call_res, expect_val);
}

TEST_F(RoutableUdsServiceFixture,
       GivenValidModelPathWhenRequestReceivedExpectNegativeResponse) {
  const auto instance = ara::core::InstanceSpecifier{"/srp/test/example"};
  auto sut = TestRoutableUdsService(
      instance,
      std::make_unique<ara::com::soc::FacadeSocketStream>(socket_mock_));
  ara::com::soc::RXCallbackStream callback_;
  EXPECT_CALL(*socket_mock_, Init(::testing::_)).Times(1);
  EXPECT_CALL(*socket_mock_, SetRXCallback(::testing::_))
      .WillOnce(::testing::Invoke(
          [&callback_](ara::com::soc::RXCallbackStream&& callback) {
            callback_ = std::move(callback);
          }));
  EXPECT_CALL(*socket_mock_, StartRXThread()).Times(1);
  EXPECT_CALL(*socket_mock_, StopRXThread()).Times(0);

  EXPECT_CALL(sut, TestMethod(::testing::_))
      .WillOnce(::testing::Return(ara::diag::MakeErrorCode(
          ara::diag::UdsDiagErrc::kGeneralReject, "")));
  const auto res = sut.Offer();
  EXPECT_TRUE(res.HasValue());
  EXPECT_TRUE(sut.IsOffered());

  const auto call_res = callback_({0x22, 0x11, 0x20});
  const std::vector<uint8_t> expect_val{0x7f, 0x22, 0x10};
  EXPECT_EQ(call_res, expect_val);
}

}  // namespace diag
}  // namespace ara
