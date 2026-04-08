/**
 * @file apogee_detector_test.cc
 * @author Wiktor Laska
 * @brief 
 * @version 0.1
 * @date 2026-04-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <gtest/gtest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "core/apogee/ApogeeDetector.h"
#include "core/apogee/KalmanFilterR7.h"
#include "core/apogee/Matrix.h"
#include "core/apogee/ut/DataLoader.h"
#include "tools/cpp/runfiles/runfiles.h"

using bazel::tools::cpp::runfiles::Runfiles;

namespace {
constexpr double apogeeError = 0.15;
}

TEST(detect_test, test_apogee_detector) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
  ASSERT_NE(runfiles, nullptr) << "Nie udalo sie zainicjowac runfiles: " << error;

  const std::string base_path = "simulation_data_apogee/10hz_1000m";

  const std::filesystem::path barometerPath = runfiles->Rlocation(base_path + "/exported_barometer_data.csv");
  const std::filesystem::path accelerometerPath = runfiles->Rlocation(base_path + "/exported_accel_data.csv");
  const std::filesystem::path configPath = runfiles->Rlocation(base_path + "/config.txt");

  checkAndGenerateConfig(configPath);
  ConfigData config = readConfig(configPath);

  const double dt = config.hz > 0 ? 1.0 / config.hz : 0.1;
  KalmanFilterR7 kf(dt);
  RealTimeApogee apogee(15, -0.5, 0.0);

  const std::vector<double> pressureData = readColumnCSV(barometerPath, 1);
  const std::vector<double> accelData = readColumnCSV(accelerometerPath, 3);
  const size_t dataSize = accelData.size();

  double height = 0.0;
  for (size_t i = 0; i < dataSize; ++i) {
    const double accel = accelData[i];
    const double pressure = pressureData[i];

    Matrix state = kf.processMeasurement(accel, pressure);
    height = state(0, 0);
    const double velocity = state(1, 0);

    apogee.update(height, velocity);
    if (apogee.isApogeeReached()) {
      break;
    }
  }
  EXPECT_NEAR(config.max_height_clean, height, config.max_height_clean * apogeeError);
}

class ApogeeFlightDataTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ApogeeFlightDataTest, CalculatesApogeeCorrectly) {
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
  ASSERT_NE(runfiles, nullptr) << "Nie udalo sie zainicjowac runfiles: " << error;

  const std::string param = GetParam();
  const std::string path_in_zip = "simulation_data_apogee/" + param;

  const std::filesystem::path barometerPath = runfiles->Rlocation(path_in_zip + "/exported_barometer_data.csv");
  const std::filesystem::path accelerometerPath = runfiles->Rlocation(path_in_zip + "/exported_accel_data.csv");
  const std::filesystem::path configPath = runfiles->Rlocation(path_in_zip + "/config.txt");

  ASSERT_TRUE(std::filesystem::exists(barometerPath)) << "BRAK PLIKU: " << barometerPath;
  ASSERT_TRUE(std::filesystem::exists(accelerometerPath)) << "BRAK PLIKU: " << accelerometerPath;

  checkAndGenerateConfig(configPath);
  ASSERT_TRUE(std::filesystem::exists(configPath)) << "BRAK PLIKU: " << configPath;

  ConfigData config = readConfig(configPath);
  const double dt = config.hz > 0 ? 1.0 / config.hz : 0.1;
  KalmanFilterR7 kf(dt);
  RealTimeApogee apogee(15, -0.5, 0.0);

  const std::vector<double> pressureData = readColumnCSV(barometerPath, 1);
  const std::vector<double> accelData = readColumnCSV(accelerometerPath, 3);
  const size_t dataSize = accelData.size();

  ASSERT_GT(dataSize, 0) << "Wczytano 0 wierszy z pliku CSV!";

  double height = 0.0;
  for (size_t i = 0; i < dataSize; ++i) {
    const double accel = accelData[i];
    const double pressure = pressureData[i];

    Matrix state = kf.processMeasurement(accel, pressure);
    height = state(0, 0);
    const double velocity = state(1, 0);

    apogee.update(height, velocity);
    if (apogee.isApogeeReached()) {
      break;
    }
  }
  EXPECT_NEAR(config.max_height_clean, height, config.max_height_clean * apogeeError);
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
