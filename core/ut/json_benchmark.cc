/**
 * @file json_benchmark.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <benchmark/benchmark.h>
#include <string>
#include <vector>
#include "core/json/simdjson_parser.h"

namespace srp {
namespace core {
namespace json {

std::string CreateJsonOfSize(size_t target_size) {
    if (target_size < 20) return R"({"a":1})";
    std::string result = "{ \"data\": [";
    while (result.size() < target_size - 5) {
        result += "123,";
    }
    if (result.back() == ',') result.pop_back();
    result += "], \"msg\": \"ok\" }";
    return result;
}

static void BM_JsonParser_CtorAndAccess(benchmark::State& state) {  // NOLINT
    const std::string json_data = CreateJsonOfSize(state.range(0));

    for (auto _ : state) {
        // Testujemy pełny cykl: Alokacja parsera + Parsowanie + Dostęp
        JsonParser parser(json_data);
        if (parser.IsValid()) {
            auto msg = parser.GetString("msg");
            benchmark::DoNotOptimize(msg);
        }
    }
    state.SetBytesProcessed(state.iterations() * state.range(0));
}

static void BM_JsonParser_AccessOnly(benchmark::State& state) {  // NOLINT
    const std::string json_data = CreateJsonOfSize(state.range(0));
    JsonParser parser(json_data);

    for (auto _ : state) {
        auto msg = parser.GetString("msg");
        auto obj = parser.GetObject("data");
        benchmark::DoNotOptimize(msg);
        benchmark::DoNotOptimize(obj);
    }
}

BENCHMARK(BM_JsonParser_CtorAndAccess)->RangeMultiplier(4)->Range(16, 10240);
BENCHMARK(BM_JsonParser_AccessOnly)->RangeMultiplier(4)->Range(16, 1024);

}  // namespace json
}  // namespace core
}  // namespace srp

BENCHMARK_MAIN();
