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


static void ParseBenchamrk(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    auto data = "$GNGGA,182658.40,5421.08022,N,01835.23691,E,1,08,2.00,120.5,M,32.8,M,,*49";
    auto res = simba::core::Nmea::Parse(data);
    const auto val = res.value();
  }
}

// Register the function as a benchmark
BENCHMARK(ParseBenchamrk)->Iterations(100);

// Run the benchmark
BENCHMARK_MAIN();
