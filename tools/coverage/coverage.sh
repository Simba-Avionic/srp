#!/bin/bash

# Uruchomienie bazel coverage i ukrycie wyjścia
bazel coverage --combined_report=lcov > /dev/null 2>&1

# Uruchomienie bazel query i ukrycie wyjścia
bazel query 'kind(cc_.*, tests(//...))' > /dev/null 2>&1

# Uruchomienie polecenia lcov i zapisanie wyniku do zmiennej
output=$(lcov --summary bazel-out/_coverage/_coverage_report.dat)

# Wyodrębnienie % pokrycia linii za pomocą grep i awk
line_coverage=$(echo "$output" | grep "lines......:" | awk '{print $2}' | sed 's/%//')

# Wyodrębnienie % pokrycia funkcji za pomocą grep i awk
function_coverage=$(echo "$output" | grep "functions..:" | awk '{print $2}' | sed 's/%//')

# Wypisanie wyników
echo "Pokrycie linii: $line_coverage%"
echo "Pokrycie funkcji: $function_coverage%"

# Zwrócenie danych
#echo "$line_coverage $function_coverage"
