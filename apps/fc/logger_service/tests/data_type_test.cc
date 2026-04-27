/**
 * @file data_type_test.cc
 * @author Krzysztof Kondracki (kondracki.christopher@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-04-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <gtest/gtest.h>

#include <algorithm>
#include <limits>
#include <string>

#include "apps/fc/logger_service/data_type.hpp"

namespace {
std::size_t CountSemicolons(const std::string& value) {
  return static_cast<std::size_t>(std::count(value.begin(), value.end(), ';'));
}
}  // namespace

TEST(DataToStringTest, ReturnsExpectedHeader) {
  srp::logger::Data_t data;
  EXPECT_EQ(
      data.get_header(),
      "TIMESTAMP;BOARD_TEMP1;BOARD_TEMP2;BOARD_TEMP3;BME_TEMP;BME_HUMIDITY;"
      "BME_ALTITUDE;CPU_USAGE;MEM_USAGE;DISK_UTILIZATION");
}

TEST(DataToStringTest, UsesZeroDefaultsWhenNoSamplesArrived) {
  srp::logger::Data_t data;
  EXPECT_EQ(data.to_string("1000"), "1000;0;0;0;0.00;0.00;0.00;0.00;0.00;0.00");
}

TEST(DataToStringTest, FormatsExpectedCsvLineWhenAllFieldsAreSet) {
  srp::logger::Data_t data;
  data.SetBoardTemp1(10);
  data.SetBoardTemp2(11);
  data.SetBoardTemp3(12);
  data.SetBmeTemp(20.5F);
  data.SetBmeHumidity(30.0F);
  data.SetBmeAltitude(100.25F);
  data.SetCpuUsage(40.0F);
  data.SetMemUsage(50.0F);
  data.SetDiskUtilization(60.0F);

  EXPECT_EQ(
      data.to_string("123"),
      "123;10;11;12;20.50;30.00;100.25;40.00;50.00;60.00");
}

TEST(DataToStringTest, UsesLatestValueAfterOverwrite) {
  srp::logger::Data_t data;
  data.SetBmeHumidity(1.0F);
  data.SetBmeHumidity(55.5F);

  EXPECT_EQ(data.to_string("x"), "x;0;0;0;0.00;55.50;0.00;0.00;0.00;0.00");
}

TEST(DataToStringTest, SupportsInt16BoundsForBoardTemperature) {
  srp::logger::Data_t data;
  data.SetBoardTemp1(std::numeric_limits<int16_t>::min());
  data.SetBoardTemp2(std::numeric_limits<int16_t>::max());

  EXPECT_EQ(data.to_string("b"), "b;-32768;32767;0;0.00;0.00;0.00;0.00;0.00;0.00");
}

TEST(DataToStringTest, PreservesCsvColumnCountForSimpleTimestamp) {
  srp::logger::Data_t data;
  const std::string line = data.to_string("ts");

  EXPECT_EQ(CountSemicolons(line), 9U);
}
