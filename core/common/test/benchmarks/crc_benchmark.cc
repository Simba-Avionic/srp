/**
 * @file crc_benchmark.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <benchmark/benchmark.h>

#include "core/common/crc_16.h"

static void CrcBenchmark_U8(benchmark::State& state) {
    for (auto _ : state) {
        uint8_t val = 240;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_U8);

static void CrcBenchmark_U16(benchmark::State& state) {
    for (auto _ : state) {
        uint16_t val = 300;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_U16);

static void CrcBenchmark_U32(benchmark::State& state) {
    for (auto _ : state) {
        uint32_t val = 33000;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_U32);

static void CrcBenchmark_U64(benchmark::State& state) {
    for (auto _ : state) {
        uint64_t val = 330000;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_U64);

static void CrcBenchmark_I8(benchmark::State& state) {
    for (auto _ : state) {
        int8_t val = 240;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_I8);

static void CrcBenchmark_I16(benchmark::State& state) {
    for (auto _ : state) {
        int16_t val = 300;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_I16);

static void CrcBenchmark_I32(benchmark::State& state) {
    for (auto _ : state) {
        int32_t val = 33000;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_I32);

static void CrcBenchmark_I64(benchmark::State& state) {
    for (auto _ : state) {
        int64_t val = 330000;
        simba::core::CRC16::calculate(val);
    }
}
BENCHMARK(CrcBenchmark_I64);

BENCHMARK_MAIN();