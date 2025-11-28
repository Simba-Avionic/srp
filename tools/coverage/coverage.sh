#!/bin/bash
export GCOV=/usr/bin/gcov-13

# Weryfikacja lcov
echo "🔍 Verifying lcov availability..."
if command -v lcov &> /dev/null; then
    echo "✅ lcov found: $(lcov --version | head -n1)"
    echo "📍 lcov path: $(which lcov)"
else
    echo "❌ WARNING: lcov not found in PATH!"
    echo "PATH: $PATH"
fi

# Uruchomienie bazel coverage i ukrycie wyjścia
bazel coverage  `bazel query 'kind(cc_test, //...)'`  -s \
  --instrument_test_targets \
  --experimental_cc_coverage \
  --combined_report=lcov \
  --instrumentation_filter="[:]" \
  --coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main

python3 tools/coverage/remove_external.py

# Uruchomienie polecenia lcov i zapisanie wyniku do zmiennej
output=$(lcov --summary bazel-out/_coverage/coverage_report.dat)

# Wyodrębnienie % pokrycia linii za pomocą grep i awk
line_coverage=$(echo "$output" | grep "lines......:" | awk '{print $2}' | sed 's/%//')
function_coverage=$(echo "$output" | grep "functions..:" | awk '{print $2}' | sed 's/%//')

# Wypisanie wyników
echo "Pokrycie linii: $line_coverage%"
echo "Pokrycie funkcji: $function_coverage%"

exit 0
