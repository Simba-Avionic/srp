#!/bin/bash

# Wykonanie polecenia coverage report -m i przechwycenie wyniku
coverage_report=$(python3-coverage report -m)

# Przechwycenie procentowego pokrycia z wyniku
total_coverage=$(echo "$coverage_report" | grep "TOTAL" | awk '{print $NF}')

echo "Całkowita procentowa pokrywalność kodu: $total_coverage"