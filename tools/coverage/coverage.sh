#!/bin/bash
export GCOV=/usr/bin/gcov-13
# Uruchomienie bazel coverage i ukrycie wyjścia
bazel coverage //... -s \
  --instrument_test_targets \
  --experimental_cc_coverage \
  --combined_report=lcov \
  --instrumentation_filter="[:],-external/[:]"
  --coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main > /dev/null 2>&1


python3 tools/coverage/remove_external.py

# Uruchomienie polecenia lcov i zapisanie wyniku do zmiennej
output=$(lcov --summary bazel-out/_coverage/coverage_report.dat)

# Wyodrębnienie % pokrycia linii za pomocą grep i awk
line_coverage=$(echo "$output" | grep "lines......:" | awk '{print $2}' | sed 's/%//')
function_coverage=$(echo "$output" | grep "functions..:" | awk '{print $2}' | sed 's/%//')

# Wypisanie wyników
echo "Pokrycie linii: $line_coverage%"
echo "Pokrycie funkcji: $function_coverage%"

if [[ -z "$line_coverage" || -z "$function_coverage" ]]; then
    echo "Błąd: Brak danych pokrycia testowego."
    exit 1
fi

# Sprawdzenie, czy pokrycie jest poniżej progu
if (( $(echo "$line_coverage < 50" | bc -l) )) || (( $(echo "$function_coverage < 50" | bc -l) )); then
    echo "Pokrycie linii ($line_coverage%) lub funkcji ($function_coverage%) jest poniżej progu."
    exit 1
fi
exit 0
# Zwrócenie danych
#echo "$line_coverage $function_coverage"
