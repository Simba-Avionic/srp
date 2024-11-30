/**
 * @file test_model_item.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <optional>
#include "ara/core/model_item.h"

namespace ara {
namespace core {

class ModelItemTest : public ::testing::TestWithParam<model::ModelSomeIp> {};

TEST_P(ModelItemTest, GetValueReturnsModelSomeIpWhenSetCorrectly) {
  const auto& expected_value = GetParam();
  ModelItem item{expected_value};
  auto result = item.GetValue<model::ModelSomeIp>();
  ASSERT_TRUE(result.has_value());
  const auto& mod = result.value();
  EXPECT_EQ(mod.com_id_, expected_value.com_id_);
  EXPECT_EQ(mod.port_, expected_value.port_);
  EXPECT_EQ(mod.instance_, expected_value.instance_);
  EXPECT_EQ(mod.major_version_, expected_value.major_version_);
  EXPECT_EQ(mod.minor_version_, expected_value.minor_version_);
  EXPECT_EQ(mod.dir_, expected_value.dir_);
}

INSTANTIATE_TEST_SUITE_P(
    ModelSomeIpValues,
    ModelItemTest,
    ::testing::Values(
        model::ModelSomeIp{"12", 1, 2, 3, 4, model::ModelSomeIp::kIn},
        model::ModelSomeIp{"34", 12, 89, 36, 18, model::ModelSomeIp::kOut},
        model::ModelSomeIp{"56", 56, 56, 56, 56, model::ModelSomeIp::kIn},
        model::ModelSomeIp{"", 0, 0, 0, 0, model::ModelSomeIp::kIn},
        model::ModelSomeIp{"asfdfisadf", 255, 255, 255, 255, model::ModelSomeIp::kOut},
        model::ModelSomeIp{"instance", 28, 586, 35, 226, model::ModelSomeIp::kIn}));

TEST(ModelItemTest, GetValueReturnsNullOption) {
  model::ModelDtc dtc("", 12, "");
  ModelItem item{dtc};
  auto result = item.GetValue<model::ModelSomeIp>();
  ASSERT_FALSE(result.has_value());
}

class ModelUDSItemTest : public ::testing::TestWithParam<model::ModelUds> {};

TEST_P(ModelUDSItemTest, GetValueReturnsModelUDSWhenSetCorrectly) {
  const auto& expected_value = GetParam();
  ModelItem item{expected_value};
  auto result = item.GetValue<model::ModelUds>();
  ASSERT_TRUE(result.has_value());
  const auto& mod = result.value();
  EXPECT_EQ(mod.service_id_, expected_value.service_id_);
  EXPECT_EQ(mod.sub_service_id_, expected_value.sub_service_id_);
  EXPECT_EQ(mod.com_id_, expected_value.com_id_);
  EXPECT_EQ(mod.com_direction_, expected_value.com_direction_);
}

INSTANTIATE_TEST_SUITE_P(
    ModelUDSValues,
    ModelUDSItemTest,
    ::testing::Values(
        model::ModelUds{12, 13, "12", ara::core::model::ModelUds::Direction::kIn},
        model::ModelUds{12, 13, "", ara::core::model::ModelUds::Direction::kOut},
        model::ModelUds{0, 0, "ara", ara::core::model::ModelUds::Direction::kIn},
        model::ModelUds{255, 255, "asdofuhasoufdh", ara::core::model::ModelUds::Direction::kOut}));

TEST(ModelUDSItemTest, GetValueReturnsNullOption) {
  model::ModelSomeIp model{"12", 1, 2, 3, 4, model::ModelSomeIp::kIn};
  ModelItem item{model};
  auto result = item.GetValue<model::ModelUds>();
  ASSERT_FALSE(result.has_value());
}

}  // namespace core
}  // namespace ara
