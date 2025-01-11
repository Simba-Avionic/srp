/**
 * @file test_error_code.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"

namespace ara {
namespace core {

// Mock dla ErrorDomain
class MockErrorDomain : public ara::core::ErrorDomain {
 public:
  MockErrorDomain() : ara::core::ErrorDomain(1) {}
  MOCK_METHOD(const std::string, Message, (CodeType), (const, override, noexcept));
  const std::string Name() const noexcept override {
    return {"MockErrorDomain"};
  }
};



TEST(ErrorCodeTest, Constructor_EnumType) {
    MockErrorDomain domain;
    ErrorDomain::SupportDataType data{"Sample Data"};

    ErrorCode error(1, domain, data);

    EXPECT_EQ(error.Value(), 1);
    EXPECT_EQ(error.SupportData(), data);
    EXPECT_EQ(&error.Domain(), &domain);
}


TEST(ErrorCodeTest, Constructor_ExplicitValues) {
    MockErrorDomain domain;
    const ErrorDomain::CodeType code = 1;
    const ErrorDomain::SupportDataType data = "100";

    ErrorCode error(code, domain, data);

    EXPECT_EQ(error.Value(), code);
    EXPECT_EQ(error.SupportData(), data);
    EXPECT_EQ(&error.Domain(), &domain);
}


TEST(ErrorCodeTest, Message_ReturnsCorrectString) {
    MockErrorDomain domain;
    constexpr ErrorDomain::CodeType code = 2;

    EXPECT_CALL(domain, Message(code))
        .WillOnce(::testing::Return("Test Error Message"));

    ErrorCode error(code, domain);

    EXPECT_EQ(error.Message(), "Test Error Message");
}

TEST(ErrorCodeTest, EqualityOperator_SameValues) {
    MockErrorDomain domain;
    constexpr ErrorDomain::CodeType code = 3;
    const ErrorDomain::SupportDataType data = "ala";

    ErrorCode error1(code, domain, data);
    ErrorCode error2(code, domain, data);

    EXPECT_TRUE(error1 == error2);
    EXPECT_FALSE(error1 != error2);
}


TEST(ErrorCodeTest, EqualityOperator_DifferentValues) {
    MockErrorDomain domain;
    constexpr ErrorDomain::CodeType code1 = 4;
    constexpr ErrorDomain::CodeType code2 = 5;
    const ErrorDomain::SupportDataType data1 = "100";
    const ErrorDomain::SupportDataType data2 = "200";

    ErrorCode error1(code1, domain, data1);
    ErrorCode error2(code2, domain, data2);

    EXPECT_FALSE(error1 == error2);
    EXPECT_TRUE(error1 != error2);
}


TEST(ErrorCodeTest, ThrowAsException_DoesNothing) {
    MockErrorDomain domain;
    constexpr ErrorDomain::CodeType code = 6;
    ErrorCode error(code, domain);
    EXPECT_NO_THROW(error.ThrowAsException());
}

}  // namespace core
}  // namespace ara
