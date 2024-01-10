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

#include <chrono> // NOLINT
#include <thread>  // NOLINT

#include "application/parm.h"

static void ParamConvertBenchmark(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    simba::core::Parm parm{"abc=123"};
    parm.GetValue();
    parm.GetName();
  }
}
// Register the function as a benchmark
BENCHMARK(ParamConvertBenchmark);
// Run the benchmark
BENCHMARK_MAIN();
