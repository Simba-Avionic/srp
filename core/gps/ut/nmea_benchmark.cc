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
    "$GNGGA,182701.80,5421.08322,N,01835.23911,E,1,05,1.10,117.5,M,29.8,M,,*3F"
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
