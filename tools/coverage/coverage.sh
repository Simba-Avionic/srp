#!/bin/bash
export GCOV=/usr/bin/gcov-13
bazel fetch
bazel build //...
bazel test //...
# Uruchomienie bazel coverage i ukrycie wyjścia
bazel coverage //... -s \
  --instrument_test_targets \
  --experimental_cc_coverage \
  --combined_report=lcov \
  --instrumentation_filter="[:],-external/[:]"
  --coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main > /dev/null 2>&1

# Uruchomienie polecenia lcov i zapisanie wyniku do zmiennej
output=$(lcov --summary bazel-out/_coverage/_coverage_report.dat)

# Wyodrębnienie % pokrycia linii za pomocą grep i awk
line_coverage=$(echo "$output" | grep "lines......:" | awk '{print $2}' | sed 's/%//')
function_coverage=$(echo "$output" | grep "functions..:" | awk '{print $2}' | sed 's/%//')


# Wypisanie wyników
echo "Pokrycie linii: $line_coverage%"
echo "Pokrycie funkcji: $function_coverage%"

if (( $(echo "$line_coverage < 50" | bc -l) )) || (( $(echo "$function_coverage < 50" | bc -l) )); then
    exit 1
fi
exit 0
# Zwrócenie danych
#echo "$line_coverage $function_coverage"
