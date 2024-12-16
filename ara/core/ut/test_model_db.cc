/**
 * @file test_model_db.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include "ara/core/model_db.h"
#include "ara/core/result.h"
#include "ara/core/core_error_domain.h"
#include "ara/core/model_item.h"
#include "ara/core/instance_specifier.h"

using namespace ara::core;  // NOLINT

class ModelDataBaseTest : public ::testing::Test {
 protected:
  ModelDataBase& db = ModelDataBase::GetInstance();

  void SetUp() override {
    db = ModelDataBase();
  }
};

TEST_F(ModelDataBaseTest, AddNewItem_Success) {
  auto instance = StringView("123");
  model::ModelUds item = model::ModelUds(12, 12, "12", model::ModelUds::kIn);
  auto res = db.AddNewItem(instance, item);
  EXPECT_TRUE(res.HasValue());
  auto val = db.ResolveInstanceSpecifier<model::ModelUds>(InstanceSpecifier("123"));
  EXPECT_TRUE(val.HasValue());
  EXPECT_EQ(item.service_id_, val.Value().service_id_);
  EXPECT_EQ(item.sub_service_id_, val.Value().sub_service_id_);
  EXPECT_EQ(item.com_id_, val.Value().com_id_);
  EXPECT_EQ(item.com_direction_, val.Value().com_direction_);
}

TEST_F(ModelDataBaseTest, InvalidInstance) {
  auto instance = StringView("123");
  model::ModelUds item = model::ModelUds(12, 12, "12", model::ModelUds::kIn);
  auto res = db.AddNewItem(instance, item);
  EXPECT_TRUE(res.HasValue());
  auto val = db.ResolveInstanceSpecifier<model::ModelUds>(InstanceSpecifier("1"));
  EXPECT_FALSE(val.HasValue());
}
TEST_F(ModelDataBaseTest, InvalidSecondType) {
  auto instance = StringView("123");
  model::ModelUds item = model::ModelUds(12, 12, "12", model::ModelUds::kIn);
  auto res = db.AddNewItem(instance, item);
  EXPECT_TRUE(res.HasValue());
  auto val = db.ResolveInstanceSpecifier<model::ModelDtc>(InstanceSpecifier("123"));
  EXPECT_FALSE(val.HasValue());
}
TEST_F(ModelDataBaseTest, AddSameInstance) {
  auto instance = StringView("123");
  model::ModelUds item = model::ModelUds(12, 12, "12", model::ModelUds::kIn);
  auto res = db.AddNewItem(instance, item);
  EXPECT_TRUE(res.HasValue());
  auto res2 = db.AddNewItem(instance, item);
  EXPECT_FALSE(res2.HasValue());
}
