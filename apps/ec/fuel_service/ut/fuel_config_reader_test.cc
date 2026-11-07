/**
 * @file fuel_config_reader_test.cc
 * @author Wiktor Müller (wiktor.muller8@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <cstdio>
#include "apps/ec/fuel_service/fuel_config_reader.hpp"

namespace srp::apps {

class FuelConfigReaderTest : public ::testing::Test {
 protected:
    const std::string kTestFilePath = "/tmp/ut_fuel_config_test.json";

    void WriteConfig(const std::string& content) {
        std::ofstream out(kTestFilePath);
        out << content;
        out.close();
    }

    void TearDown() override {
        std::remove(kTestFilePath.c_str());
    }
};

TEST_F(FuelConfigReaderTest, LoadsValidConfigWithMultipleTanks) {
    const std::string valid_json = R"({
        "tanks": [
            {
                "tank_id": 0,
                "tank_name": "Oxidizer",
                "refueling_type": "MASS",
                "min_gs_pressure": 4000,
                "max_gs_pressure": 6000,
                "test_pressure": 400,
                "test_max_drop_percent": 10,
                "max_safe_pressure": 6500,
                "max_tanking_time_s": 60,
                "valves": {
                    "gs_main": 1,
                    "rocket_vent": 61,
                    "rocket_dump": 62
                }
            },
            {
                "tank_id": 1,
                "tank_name": "PFS",
                "refueling_type": "PRESSURE",
                "min_gs_pressure": 2000,
                "max_gs_pressure": 3000,
                "test_pressure": 200,
                "test_max_drop_percent": 5,
                "max_safe_pressure": 3500,
                "max_tanking_time_s": 30,
                "valves": {
                    "gs_main": 2,
                    "rocket_vent": null,
                    "rocket_dump": 64
                }
            }
        ]
    })";

    WriteConfig(valid_json);
    auto configs_opt = FuelConfigReader::LoadConfig(kTestFilePath);

    ASSERT_TRUE(configs_opt.has_value());
    auto configs = configs_opt.value();
    ASSERT_EQ(configs.size(), 2);

    EXPECT_EQ(configs[0].tank_id, 0);
    EXPECT_EQ(configs[0].tank_name, "Oxidizer");
    EXPECT_EQ(configs[0].refueling_type, RefuelingType_t::MASS);
    EXPECT_EQ(configs[0].gs_main_valve_id, 1);
    EXPECT_TRUE(configs[0].rocket_vent_valve_id.has_value());
    EXPECT_EQ(configs[0].rocket_vent_valve_id.value(), 61);

    EXPECT_EQ(configs[1].tank_id, 1);
    EXPECT_EQ(configs[1].tank_name, "PFS");
    EXPECT_EQ(configs[1].refueling_type, RefuelingType_t::PRESSURE);
    EXPECT_EQ(configs[1].gs_main_valve_id, 2);
    EXPECT_FALSE(configs[1].rocket_vent_valve_id.has_value());
}

TEST_F(FuelConfigReaderTest, FailsOnMissingFile) {
    auto configs_opt = FuelConfigReader::LoadConfig("/tmp/non_existent_file.json");
    EXPECT_FALSE(configs_opt.has_value());
}

TEST_F(FuelConfigReaderTest, FailsOnInvalidJsonSyntax) {
    WriteConfig("{ broken_json: [ }");
    auto configs_opt = FuelConfigReader::LoadConfig(kTestFilePath);
    EXPECT_FALSE(configs_opt.has_value());
}

TEST_F(FuelConfigReaderTest, FailsOnMissingTanksArray) {
    WriteConfig(R"({ "wrong_key": [] })");
    auto configs_opt = FuelConfigReader::LoadConfig(kTestFilePath);
    EXPECT_FALSE(configs_opt.has_value());
}

TEST_F(FuelConfigReaderTest, FailsOnMissingRequiredField) {
    const std::string missing_id_json = R"({
        "tanks": [{
            "tank_name": "Oxidizer",
            "refueling_type": "MASS",
            "min_gs_pressure": 4000,
            "max_gs_pressure": 6000,
            "test_pressure": 400,
            "test_max_drop_percent": 10,
            "max_safe_pressure": 6500,
            "max_tanking_time_s": 60,
            "valves": {"gs_main": 1, "rocket_dump": 62}
        }]
    })";

    WriteConfig(missing_id_json);
    auto configs_opt = FuelConfigReader::LoadConfig(kTestFilePath);
    EXPECT_FALSE(configs_opt.has_value());
}

TEST_F(FuelConfigReaderTest, FailsOnInvalidRefuelingType) {
    const std::string invalid_type_json = R"({
        "tanks": [{
            "tank_id": 0,
            "tank_name": "Oxidizer",
            "refueling_type": "MAGIC_LIQUID", 
            "min_gs_pressure": 4000,
            "max_gs_pressure": 6000,
            "test_pressure": 400,
            "test_max_drop_percent": 10,
            "max_safe_pressure": 6500,
            "max_tanking_time_s": 60,
            "valves": {"gs_main": 1, "rocket_dump": 62}
        }]
    })";

    WriteConfig(invalid_type_json);
    auto configs_opt = FuelConfigReader::LoadConfig(kTestFilePath);
    EXPECT_FALSE(configs_opt.has_value());
}

}  // namespace srp::apps
