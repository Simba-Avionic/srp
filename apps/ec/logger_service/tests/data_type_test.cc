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
    data_.SetTemp2(2);
    data_.SetTemp3(3);
    data_.SetTankPress(4.0f);
    data_.SetTankDPress(1.22f);
    data_.SetSysStatus({5.0f, 6.0f, 7.0f});
    EXPECT_EQ(data_.to_string("12"), "12;1;2;3;4.00;1.22;6.00;5.00;7.00");
}

