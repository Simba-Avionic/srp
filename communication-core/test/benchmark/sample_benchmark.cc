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

#include <chrono>  // NOLINT
#include <thread>  // NOLINT

static void BM_SomeFunction(benchmark::State& state) {  // NOLINT
  // Perform setup here
  for (auto _ : state) {
    // This code gets timed
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SomeFunction);
// Run the benchmark
BENCHMARK_MAIN();
