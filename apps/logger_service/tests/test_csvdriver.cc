/**
 * @file test_csvdriver.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief testing csvdriver
 * @version 0.1
 * @date 2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "gtest/gtest.h"
#include "apps/logger_service/csvdriver/csvdriver.h"

namespace {
    std::string fileName = "test_log.csv";
}

class InitTest : public ::testing::TestWithParam<std::string>{
};

INSTANTIATE_TEST_SUITE_P(InitTestParameters, InitTest,
    ::testing::Values(
        "TIMESTAMP;MAIN_VALVE;VENT_VALVE;PRIMER",
        "TIMESTAMP;NOZLE_PRESS;TANK_PRESS;TANK_TEMP_1;TANK_TEMP_2;TANK_TEMP_3;TANK_D_PRESS",
        "Testowanie Inita halo halo"
    )
);

TEST_P(InitTest, FirstLineTest) {
    simba::logger::CSVDriver csv_(fileName);
    std::string firstLine = GetParam();
    csv_.Init(firstLine);
    std::ifstream file(fileName);
    std::string fileContent;
    std::getline(file, fileContent);
    file.close();
    EXPECT_EQ(fileContent, firstLine);
    std::remove(fileName.c_str());
}

class WriteLineActTest : public ::testing::TestWithParam<std::tuple<
    double, simba::logger::ActuatorData, std::string>>{
};

INSTANTIATE_TEST_SUITE_P(WriteLineActTestParameters, WriteLineActTest,
    ::testing::Values(
        std::make_tuple(0, simba::logger::ActuatorData{{0}, {0}, {0}}, "0;0;0;0;"),
        std::make_tuple(1, simba::logger::ActuatorData{{255}, {255}, {255}}, "1;255;255;255;"),
        std::make_tuple(2, simba::logger::ActuatorData{{32}, {14}, {52}}, "2;32;14;52;"),
        std::make_tuple(2, simba::logger::ActuatorData{{}, {14}, {52}}, "2;;14;52;"),
        std::make_tuple(2, simba::logger::ActuatorData{{32}, {}, {52}}, "2;32;;52;"),
        std::make_tuple(2, simba::logger::ActuatorData{{32}, {14}, {}}, "2;32;14;;")
    )
);

TEST_P(WriteLineActTest, WriteActTest) {
    simba::logger::CSVDriver csv_(fileName);
    auto params = GetParam();
    double timestamp = std::get<0>(params);
    auto Actuators = std::get<1>(params);
    std::string expected = std::get<2>(params);
    csv_.WriteLine(Actuators, timestamp);
    std::ifstream file(fileName);
    std::string fileContent;
    std::getline(file, fileContent);
    file.close();
    EXPECT_EQ(fileContent, expected);
    std::remove(fileName.c_str());
}

class WriteLineSensTest : public ::testing::TestWithParam<std::tuple<
    double, simba::logger::SensorData, std::string>>{
};

INSTANTIATE_TEST_SUITE_P(WriteLineSensTestParameters, WriteLineSensTest,
    ::testing::Values(
        std::make_tuple(0, simba::logger::SensorData{{0}, {0}, {0}, {0}, {0}, {0}}, "0;0;0;0;0;0;0;"),
        std::make_tuple(1, simba::logger::SensorData{{255}, {255}, {255},
            {255}, {255}, {255}}, "1;255;255;255;255;255;255;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {14}, {52}, {104}, {45}, {200}}, "2;32;14;52;104;45;200;"),
        std::make_tuple(2, simba::logger::SensorData{{}, {14}, {52}, {104}, {45}, {200}}, "2;;14;52;104;45;200;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {}, {52}, {104}, {45}, {200}}, "2;32;;52;104;45;200;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {14}, {}, {104}, {45}, {200}}, "2;32;14;;104;45;200;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {14}, {52}, {}, {45}, {200}}, "2;32;14;52;;45;200;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {14}, {52}, {104}, {}, {200}}, "2;32;14;52;104;;200;"),
        std::make_tuple(2, simba::logger::SensorData{{32}, {14}, {52}, {104}, {45}, {}}, "2;32;14;52;104;45;;")
    )
);

TEST_P(WriteLineSensTest, WriteSensTest) {
    simba::logger::CSVDriver csv_(fileName);
    auto params = GetParam();
    double timestamp = std::get<0>(params);
    auto Actuators = std::get<1>(params);
    std::string expected = std::get<2>(params);
    csv_.WriteLine(Actuators, timestamp);
    std::ifstream file(fileName);
    std::string fileContent;
    std::getline(file, fileContent);
    file.close();
    EXPECT_EQ(fileContent, expected);
    std::remove(fileName.c_str());
}
