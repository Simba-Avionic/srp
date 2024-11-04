#!/bin/bash

cpp_files=$(find . -type f \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -not -path "./libdoip/*" -not -path "./tools/diag_app_frontend/*")

output=$(cpplint --filter=-build/include_subdir --extensions=cc,cpp,h,hpp --linelength=120 $cpp_files 2>&1)

# Zliczanie błędów z wyjścia
total_errors=$(echo "$output" | grep -o 'Total errors found: [0-9]*' | grep -o '[0-9]*')

# Sprawdzamy, czy wystąpiły błędy
if [[ -z "$total_errors" ]]; then
    total_errors=0  # Ustaw na 0, jeśli nie znaleziono błędów
fi

if [[ $total_errors -gt 0 ]]; then
    echo "Too many errors ($total_errors), failing the build."
    exit 1
else
    echo "Acceptable number of errors ($total_errors)."
fi