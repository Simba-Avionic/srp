#include <benchmark/benchmark.h>

#include <chrono>
#include <thread>

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
