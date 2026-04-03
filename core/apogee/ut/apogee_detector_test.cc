#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <filesystem>
#include "tools/cpp/runfiles/runfiles.h"

#include "core/apogee/Matrix.h"
#include "core/apogee/KalmanFilterR7.h"
#include "core/apogee/ApogeeDetector.h"
#include "core/apogee/ut/DataLoader.h"

using bazel::tools::cpp::runfiles::Runfiles;

namespace
{
    static constexpr auto apoogeeError = 0.15;
}

TEST(detect_test, test_apogee_detecotr)
{
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
    ASSERT_NE(runfiles, nullptr) << "Nie udalo sie zainicjowac runfiles: " << error;

    std::filesystem::path base_path = runfiles->Rlocation("moje_wielkie_dane/data/10hz_1000m");

    std::filesystem::path barometerPath = base_path / "exported_barometer_data.csv";
    std::filesystem::path accelerometerPath = base_path / "exported_accel_data.csv";
    std::filesystem::path configPath = base_path / "config.txt";

    checkAndGenerateConfig(configPath);
    ConfigData config = readConfig(configPath);
    const double dt = config.hz > 0 ? 1.0 / config.hz : 0.1;
    KalmanFilterR7 kf(dt);
    RealTimeApogee apogee(15, -0.5, 0.0);

    std::vector<double> pressureData = readColumnCSV(barometerPath, 1);
    std::vector<double> accelData = readColumnCSV(accelerometerPath, 3);
    size_t dataSize = accelData.size();

    double height = 0.0;
    for (size_t i = 0; i < dataSize; ++i)
    {
        double accel = accelData[i];
        double pressure = pressureData[i];
        Matrix state = kf.processMeasurement(accel, pressure);
        height = state(0, 0);
        double velocity = state(1, 0);
        apogee.update(height, velocity);
        if (apogee.isApogeeReached())
        {
            break;
        }
    }
    EXPECT_NEAR(config.max_height_clean, height, 1.0);
}

class ApogeeFlightDataTest : public ::testing::TestWithParam<std::string>
{
};

TEST_P(ApogeeFlightDataTest, CalculatesApogeeCorrectly)
{
    std::string error;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
    ASSERT_NE(runfiles, nullptr) << "Nie udalo sie zainicjowac runfiles: " << error;

    std::string param = GetParam();
    std::string path_in_zip = "simulation_data_apogee/" + param;

    std::filesystem::path barometerPath = runfiles->Rlocation(path_in_zip + "/exported_barometer_data.csv");
    std::filesystem::path accelerometerPath = runfiles->Rlocation(path_in_zip + "/exported_accel_data.csv");
    std::filesystem::path configPath = runfiles->Rlocation(path_in_zip + "/config.txt");

    ASSERT_TRUE(std::filesystem::exists(barometerPath)) << "BRAK PLIKU: " << barometerPath;
    ASSERT_TRUE(std::filesystem::exists(accelerometerPath)) << "BRAK PLIKU: " << accelerometerPath;
    
    checkAndGenerateConfig(configPath);
    ASSERT_TRUE(std::filesystem::exists(configPath)) << "BRAK PLIKU: " << configPath;
    ConfigData config = readConfig(configPath);
    const double dt = config.hz > 0 ? 1.0 / config.hz : 0.1;
    KalmanFilterR7 kf(dt);
    RealTimeApogee apogee(15, -0.5, 0.0);

    std::vector<double> pressureData = readColumnCSV(barometerPath, 1);
    std::vector<double> accelData = readColumnCSV(accelerometerPath, 3);
    size_t dataSize = accelData.size();

    ASSERT_GT(dataSize, 0) << "Wczytano 0 wierszy z pliku CSV!";

    double height = 0.0;
    for (size_t i = 0; i < dataSize; ++i)
    {
        double accel = accelData[i];
        double pressure = pressureData[i];
        Matrix state = kf.processMeasurement(accel, pressure);
        height = state(0, 0);
        double velocity = state(1, 0);
        apogee.update(height, velocity);
        if (apogee.isApogeeReached())
        {
            break;
        }
    }
    EXPECT_NEAR(config.max_height_clean, height, config.max_height_clean * apoogeeError);
}
INSTANTIATE_TEST_SUITE_P(
    FlightDataSimulation,
    ApogeeFlightDataTest,
    ::testing::Values(
        "10hz_1000m",
        "10hz_2000m",
        "10hz_4000m",
        "10hz_5000m",
        "10hz_6000m",
        "100hz_1000m",
        "100hz_2000m",
        "100hz_3000m",
        "100hz_4000m",
        "100hz_5000m",
        "100hz_6000m",
        "1000hz_1000m",
        "1000hz_2000m",
        "1000hz_5000m",
        "1000hz_6000m"));