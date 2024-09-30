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

#include "ara/core/result.h"

static std::optional<std::vector<uint8_t>> GetOptional(uint8_t a) {
  std::vector<uint8_t> res{1,   2, 3,   4,  5,   6,   7,  8,   9,  0,
                           114, 2, 12,  4,  124, 12,  34, 134, 12, 34,
                           123, 4, 123, 12, 34,  123, 4,  132, 1};
  res.push_back(a);
  return res;
}

static ara::core::Result<std::vector<uint8_t>> GetResult(uint8_t a) {
  std::vector<uint8_t> res{1,   2, 3,   4,  5,   6,   7,  8,   9,  0,
                           114, 2, 12,  4,  124, 12,  34, 134, 12, 34,
                           123, 4, 123, 12, 34,  123, 4,  132, 1};
  res.push_back(a);
  return res;
}

static void OptionalBenchamrk(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    const auto res = GetOptional(132);
    const auto flag = res.has_value();
    const auto value = res.value();
    std::ignore = flag;
    std::ignore = value;
  }
}
static void ResultBenchamrk(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    const auto res = GetResult(132);
    const auto flag = res.HasValue();
    const auto value = res.Value();
    std::ignore = flag;
    std::ignore = value;
  }
}
// Register the function as a benchmark
BENCHMARK(OptionalBenchamrk);
BENCHMARK(ResultBenchamrk);

// Run the benchmark
BENCHMARK_MAIN();
