/**
 * @file data_type_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>

#include "apps/ec/logger_service/data_type.hpp"

TEST(DataToStringTest, DataToStringTests) {
    srp::logger::Data_t data_;
    data_.SetTemp1(1);
    data_.SetTemp2(1);
    data_.SetTemp3(1);
    data_.SetTankPress(1.0);
    data_.SetTankDPress(1.22);
    EXPECT_EQ(data_.to_string("12"), "12;1;1;1;1;1.22;");
}
