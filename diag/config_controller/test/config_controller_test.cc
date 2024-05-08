/**
 * @file config_controller_test.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "diag/config_controller/config_controller.h"

#include <gtest/gtest.h>

#include "core/common/error_code.h"

TEST(CONFIGCONTROLLER, InsertNewObject) {
  simba::diag::config::ConfigController sut{};
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertRequireObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new", "123456789"));
}

TEST(CONFIGCONTROLLER, InsertDoubleObject) {
  simba::diag::config::ConfigController sut{};
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertRequireObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kError,
            sut.InsertRequireObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kError,
            sut.InsertProvideObject("new", "123456789"));
}

TEST(CONFIGCONTROLLER, FindExistObject) {
  simba::diag::config::ConfigController sut{};
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertRequireObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk, sut.InsertRequireObject("new2", "1"));
  EXPECT_EQ(simba::core::ErrorCode::kOk, sut.InsertRequireObject("new3", "4"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new3", "1232129"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new4", "129"));
  auto res = sut.FindProvideObject("new");
  EXPECT_TRUE(res.has_value());
  EXPECT_EQ("123456789", res.value());
  res = sut.FindRequireObject("new");
  EXPECT_TRUE(res.has_value());
  EXPECT_EQ("123456789", res.value());
}

TEST(CONFIGCONTROLLER, FindNotExistObject) {
  simba::diag::config::ConfigController sut{};
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertRequireObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk, sut.InsertRequireObject("new2", "1"));
  EXPECT_EQ(simba::core::ErrorCode::kOk, sut.InsertRequireObject("new3", "4"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new", "123456789"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new3", "1232129"));
  EXPECT_EQ(simba::core::ErrorCode::kOk,
            sut.InsertProvideObject("new4", "129"));
  auto res = sut.FindProvideObject("not_exist");
  EXPECT_FALSE(res.has_value());
  res = sut.FindRequireObject("not_exist");
  EXPECT_FALSE(res.has_value());
}