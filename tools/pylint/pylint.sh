#!/bin/bash

py_files=$(find tools -name "*.py")
total_errors=0

for file in $py_files; do
    errors=$(pylint --max-line-length=120 $file | wc -l)
    if [[ $errors -gt 1 ]]; then
        total_errors=$((total_errors + errors - 1))
    fi
done

if [[ $total_errors -gt 0 ]]; then
    echo "Too many errors ($total_errors), failing the build."
    exit 1
else
    echo "Acceptable number of errors ($total_errors)."
fi
