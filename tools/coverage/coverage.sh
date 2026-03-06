#!/bin/bash
export GCOV=/usr/bin/gcov-13

if ! command -v bazel &> /dev/null; then
    echo "Bazel is not installed"
    exit 1
fi


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

if [ $? -ne 0 ]; then
    echo "❌ Bazel coverage failed. Please inspect the failing tests above."
    exit 1
fi

python3 tools/coverage/remove_external.py

# Uruchomienie polecenia lcov i zapisanie wyniku do zmiennej
output=$(lcov --summary bazel-out/_coverage/coverage_report.dat)

# Wyodrębnienie % pokrycia linii za pomocą grep i awk
line_coverage=$(echo "$output" | grep "lines......:" | awk '{print $2}' | sed 's/%//')
function_coverage=$(echo "$output" | grep "functions..:" | awk '{print $2}' | sed 's/%//')

# Wypisanie wyników
echo "Pokrycie linii: $line_coverage%"
echo "Pokrycie funkcji: $function_coverage%"

# Wymagania
required_line_coverage=65
required_function_coverage=65

# Sprawdzenie wymagań dla floatów
if ! awk -v val="$line_coverage" -v req="$required_line_coverage" 'BEGIN {exit !(val >= req)}'; then
    echo "❌ Błąd: pokrycie linii (${line_coverage}%) jest mniejsze niż wymagane ${required_line_coverage}%."
    exit 1
fi

if ! awk -v val="$function_coverage" -v req="$required_function_coverage" 'BEGIN {exit !(val >= req)}'; then
    echo "❌ Błąd: pokrycie funkcji (${function_coverage}%) jest mniejsze niż wymagane ${required_function_coverage}%."
    exit 1
fi

echo "✅ Pokrycie spełnia wymagania."
exit 0

