/**
 * @file sample_benchmark.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <benchmark/benchmark.h>

#include <optional>
#include <vector>

#include "core/gps/nmea.hpp"


static void ParseBenchmark(benchmark::State& state) {  // NOLINT
  std::vector<std::string> test_data = {
    "$GNGGA,182658.40,5421.08022,N,01835.23691,E,1,08,2.00,120.5,M,32.8,M,,*49",
    "$GNGGA,182659.00,5421.08122,N,01835.23711,E,1,07,1.50,119.5,M,31.8,M,,*5A",
    "$GNGGA,182700.20,5421.08222,N,01835.23811,E,1,06,1.20,118.5,M,30.8,M,,*4C",
    "$GNGGA,182701.80,5421.08322,N,01835.23911,E,1,05,1.10,117.5,M,29.8,M,,*3F",
    "$GNGGA,182702.40,5421.08422,N,01835.24011,E,1,05,1.00,116.5,M,28.8,M,,*3D",
    "$GNGGA,182703.00,5421.08522,N,01835.24111,E,1,06,0.90,115.4,M,27.7,M,,*3F",
    "$GNGGA,182703.60,5421.08622,N,01835.24211,E,1,06,0.80,114.3,M,26.6,M,,*39",
    "$GNGGA,182704.20,5421.08722,N,01835.24311,E,1,07,0.70,113.2,M,25.5,M,,*3E",
    "$GNGGA,182704.80,5421.08822,N,01835.24411,E,1,08,0.60,112.1,M,24.4,M,,*3A",
    "$GNGGA,182705.40,5421.08922,N,01835.24511,E,1,08,0.50,111.0,M,23.3,M,,*3F",
    "$GNGGA,182706.00,5421.09022,N,01835.24611,E,1,09,0.50,110.0,M,22.2,M,,*30",
    "$GNGGA,182706.60,5421.09122,N,01835.24711,E,1,09,0.40,109.0,M,21.1,M,,*32",
    "$GNGGA,182707.20,5421.09222,N,01835.24811,E,1,10,0.40,108.0,M,20.0,M,,*33",
    "$GNGGA,182707.80,5421.09322,N,01835.24911,E,1,10,0.30,107.0,M,19.0,M,,*3D"
  };

  for (auto _ : state) {
    for (const auto& data : test_data) {
      auto res = srp::core::Nmea::Parse(data);
      benchmark::DoNotOptimize(res.value());
    }
  }
}

// Register benchmark
BENCHMARK(ParseBenchmark)->Iterations(1000);
// Run the benchmark
BENCHMARK_MAIN();
