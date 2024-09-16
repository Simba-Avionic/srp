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

#include "ara/com/someip/HeaderStructure.h"
#include "ara/com/someip/someip_frame.h"
#include "communication-core/someip/factory/someip_header_factory.h"
#include "communication-core/someip/factory/someip_message_factory.h"
#include "communication-core/someip/someip_header.h"

static void HeaderStructureBenchmark(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    ara::com::someip::HeaderStructure sut{};
    const auto vec_r =
        ara::com::Convert2Vector<ara::com::someip::HeaderStructure>::Conv(sut);

    const auto new_sut_r =
        ara::com::Convert<ara::com::someip::HeaderStructure>::Conv(vec_r);
    const auto new_sut = new_sut_r.Value();
  }
}
static void NewSomeIpHeaderBenchmark(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    ara::com::someip::HeaderStructure header{};
    const auto sut =
        ara::com::someip::SomeipFrame::MakeFrame(header);
    const auto vec_r = sut.GetRaw();
    const auto sut2 =
        ara::com::someip::SomeipFrame::MakeFrame(vec_r);
  }
}
static void SomeIpHeaderBenchmark(benchmark::State& state) {  // NOLINT
  for (auto _ : state) {
    simba::com::someip::SomeIpHeader sut{};
    const auto vec_r = sut.GetBuffor();
    simba::com::someip::SomeIpHeader new_sut{};
    new_sut.SetBuffor(vec_r);
  }
}
static void NewSomeIpHeaderWithPayloadBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                       21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    ara::com::someip::HeaderStructure header{};
    const auto sut = ara::com::someip::SomeipFrame::MakeFrame(header, payload);
    const auto vec_r = sut.GetRaw();
    const auto sut2 = ara::com::someip::SomeipFrame::MakeFrame(vec_r);
  }
}
static void SomeIpHeaderWithPayloadBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                       21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    simba::com::someip::SomeIpHeader sut{};
    auto vec_r = sut.GetBuffor();
    vec_r.insert(vec_r.end(), payload.begin(), payload.end());
    simba::com::someip::SomeIpHeader new_sut{};
    new_sut.SetBuffor(vec_r);
  }
}
static void NewSomeIpHeaderWithPayloadBigBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1,  2,  3,  4,
        5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10};
    ara::com::someip::HeaderStructure header{};
    const auto sut = ara::com::someip::SomeipFrame::MakeFrame(header, payload);
    const auto vec_r = sut.GetRaw();
    const auto sut2 = ara::com::someip::SomeipFrame::MakeFrame(vec_r);
  }
}
static void SomeIpHeaderWithPayloadBigBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1,  2,  3,  4,
        5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 1,  2,  3,  4,  5,  6,  7,  8,
        9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10};
    simba::com::someip::SomeIpHeader sut{};
    auto vec_r = sut.GetBuffor();
    vec_r.insert(vec_r.end(), payload.begin(), payload.end());
    simba::com::someip::SomeIpHeader new_sut{};
    new_sut.SetBuffor(vec_r);
  }
}
static void NewSomeIpHeaderWithPayloadSmallBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ara::com::someip::HeaderStructure header{};
    const auto sut = ara::com::someip::SomeipFrame::MakeFrame(header, payload);
    const auto vec_r = sut.GetRaw();
    const auto sut2 = ara::com::someip::SomeipFrame::MakeFrame(vec_r);
  }
}
static void SomeIpHeaderWithPayloadSmallBenchmark(
    benchmark::State& state) {  // NOLINT

  for (auto _ : state) {
    const std::vector<uint8_t> payload{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    simba::com::someip::SomeIpHeader sut{};
    auto vec_r = sut.GetBuffor();
    vec_r.insert(vec_r.end(), payload.begin(), payload.end());
    simba::com::someip::SomeIpHeader new_sut{};
    new_sut.SetBuffor(vec_r);
  }
}
// Register the function as a benchmark
BENCHMARK(HeaderStructureBenchmark);
BENCHMARK(SomeIpHeaderBenchmark);
BENCHMARK(NewSomeIpHeaderBenchmark);
BENCHMARK(NewSomeIpHeaderWithPayloadBigBenchmark);
BENCHMARK(SomeIpHeaderWithPayloadBigBenchmark);
BENCHMARK(NewSomeIpHeaderWithPayloadBenchmark);
BENCHMARK(SomeIpHeaderWithPayloadBenchmark);
BENCHMARK(NewSomeIpHeaderWithPayloadSmallBenchmark);
BENCHMARK(SomeIpHeaderWithPayloadSmallBenchmark);
// Run the benchmark
BENCHMARK_MAIN();
