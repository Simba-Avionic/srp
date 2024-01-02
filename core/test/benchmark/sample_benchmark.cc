#include <benchmark/benchmark.h>

#include <chrono>
#include <thread>

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
