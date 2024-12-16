/**
 * @file test_instance_specifire.cc
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
#include "ara/core/instance_specifier.h"
#include "ara/core/core_error_domain.h"

using namespace ara::core;  // NOLINT

// Parametryzowane testy dla operatorów porównania i metod
class InstanceSpecifierTest : public ::testing::TestWithParam<std::tuple<StringView, StringView, bool>> {};

TEST_P(InstanceSpecifierTest, EqualityOperator) {
    StringView identifier1 = std::get<0>(GetParam());
    StringView identifier2 = std::get<1>(GetParam());
    bool expectedEquality = std::get<2>(GetParam());
    InstanceSpecifier instance1(identifier1);
    InstanceSpecifier instance2(identifier2);

    EXPECT_EQ((instance1 == instance2), expectedEquality);
}

TEST_P(InstanceSpecifierTest, NotEqualOperator) {
    StringView identifier1 = std::get<0>(GetParam());
    StringView identifier2 = std::get<1>(GetParam());
    InstanceSpecifier instance1(identifier1);
    InstanceSpecifier instance2(identifier2);
    EXPECT_EQ((instance1 != instance2), !(instance1 == instance2));
}

TEST_P(InstanceSpecifierTest, LessThanOperator) {
    StringView identifier1 = std::get<0>(GetParam());
    StringView identifier2 = std::get<1>(GetParam());

    InstanceSpecifier instance1(identifier1);
    InstanceSpecifier instance2(identifier2);

    EXPECT_TRUE((instance1 < instance2) == (identifier1 < identifier2));
}

// Parametryzowane testy dla funkcji Create
class CreateInstanceSpecifierTest : public ::testing::TestWithParam<std::tuple<StringView, bool, StringView>> {};

TEST_P(CreateInstanceSpecifierTest, CreateWithValidIdentifier) {
    StringView identifier = std::get<0>(GetParam());
    bool expectedHasValue = std::get<1>(GetParam());
    StringView expectedValue = std::get<2>(GetParam());

    auto result = InstanceSpecifier::Create(identifier);
    EXPECT_EQ(result.HasValue(), expectedHasValue);
    if (expectedHasValue) {
        EXPECT_EQ(result.Value().ToString(), expectedValue);
    }
}

TEST_P(InstanceSpecifierTest, ToStringMethod) {
    StringView identifier = std::get<0>(GetParam());
    InstanceSpecifier instance(identifier);
    EXPECT_EQ(instance.ToString(), identifier);
}

// Definicja zestawu danych dla parametrów
INSTANTIATE_TEST_SUITE_P(
    EqualityTests,
    InstanceSpecifierTest,
    ::testing::Values(
        std::make_tuple("modelA", "modelA", true),
        std::make_tuple("modelA", "modelB", false),
        std::make_tuple("modelA", "modelA", true),
        std::make_tuple("", "", true),
        std::make_tuple("model1", "model2", false),
        std::make_tuple("123", "123", true),
        std::make_tuple("ModelA", "modelA", false)
    )
);

INSTANTIATE_TEST_SUITE_P(
    CreateTests,
    CreateInstanceSpecifierTest,
    ::testing::Values(
        std::make_tuple("validIdentifier", true, "validIdentifier"),
        std::make_tuple("123valid", true, "123valid"),
        std::make_tuple("", true, ""),
        std::make_tuple("", true, ""),
        std::make_tuple("ValidIdentifier", true, "ValidIdentifier"),
        std::make_tuple("Invalid123", true, "Invalid123")
    )
);

TEST(InstanceSpecifierCopyTest, CopyConstructor) {
    StringView identifier = "testModel";
    InstanceSpecifier original(identifier);
    InstanceSpecifier copy(original);

    EXPECT_EQ(copy.ToString(), original.ToString());
}

TEST(InstanceSpecifierMoveTest, MoveConstructor) {
    StringView identifier = "testModel";
    InstanceSpecifier original(identifier);
    InstanceSpecifier moved(std::move(original));

    EXPECT_EQ(moved.ToString(), identifier);
}

TEST(InstanceSpecifierCopyTest, CopyAssignment) {
    StringView identifier = "testModel";
    InstanceSpecifier original(identifier);
    InstanceSpecifier copy = original;

    EXPECT_EQ(copy.ToString(), original.ToString());
}

TEST(InstanceSpecifierMoveTest, MoveAssignment) {
    StringView identifier = "testModel";
    InstanceSpecifier original(identifier);
    InstanceSpecifier moved = std::move(original);

    EXPECT_EQ(moved.ToString(), identifier);
}

TEST(InstanceSpecifierOperatorTest, NotEqualWithStringView) {
    StringView identifier = "modelA";
    StringView otherIdentifier = "modelB";
    InstanceSpecifier instance(identifier);

    EXPECT_TRUE(instance != otherIdentifier);
    EXPECT_FALSE(instance != identifier);
}

TEST(InstanceSpecifierEdgeCaseTest, ToStringEmptyIdentifier) {
    StringView emptyIdentifier = "";
    InstanceSpecifier instance(emptyIdentifier);

    EXPECT_EQ(instance.ToString(), emptyIdentifier);
}

TEST(InstanceSpecifierEdgeCaseTest, ToStringLongIdentifier) {
    StringView longIdentifier = "ThisIsAVeryLongIdentifierThatShouldStillWorkCorrectly";
    InstanceSpecifier instance(longIdentifier);

    EXPECT_EQ(instance.ToString(), longIdentifier);
}

TEST(InstanceSpecifierAssignmentTest, CopyAssignmentOperator) {
    StringView identifier1 = "modelA";
    StringView identifier2 = "modelB";

    InstanceSpecifier instance1(identifier1);
    InstanceSpecifier instance2(identifier2);

    instance1 = instance2;

    EXPECT_EQ(instance1.ToString(), identifier2);
    EXPECT_EQ(instance2.ToString(), identifier2);
}

TEST(InstanceSpecifierAssignmentTest, MoveAssignmentOperator) {
    StringView identifier1 = "modelA";
    StringView identifier2 = "modelB";

    InstanceSpecifier instance1(identifier1);
    InstanceSpecifier instance2(identifier2);

    instance1 = std::move(instance2);

    EXPECT_EQ(instance1.ToString(), identifier2);
}

TEST(InstanceSpecifierComparisonTest, EqualWithStringView) {
    StringView identifier = "modelA";

    InstanceSpecifier instance(identifier);

    EXPECT_TRUE(instance == identifier);  // NOLINT
    EXPECT_FALSE(instance == "modelB");  // NOLINT
}

TEST(CreateInstanceSpecifierTest, CreateWithNumericIdentifier) {
    StringView numericIdentifier = "123";

    auto result = InstanceSpecifier::Create(numericIdentifier);

    EXPECT_FALSE(result.HasValue());
    EXPECT_EQ(result.Error().Value(), static_cast<int>(CoreErrc::kInvalidMetaModelShortname));
}
