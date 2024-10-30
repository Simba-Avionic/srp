#!/bin/bash

# Znajdź wszystkie pliki do przetworzenia i zapisz je w zmiennej.
cpp_files=$(find . -path "./libdoip" -prune -o -path "./tools/diag_app_frontend" -prune -o \( -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print)

# Uruchom cpplint dla wszystkich plików jednocześnie i policz liczbę błędów.
total_errors=$(cpplint --filter=-build/include_subdir --extensions=cc,cpp,h,hpp --linelength=120 $cpp_files 2>&1 | tee >(wc -l) | tail -n1)

# Odejmij 1, aby dostosować wynik do warunku.
total_errors=$((total_errors - 1))

# Sprawdź liczbę błędów i wyświetl odpowiedni komunikat.
if [[ $total_errors -gt 0 ]]; then
    echo "Too many errors ($total_errors), failing the build."
    exit 1
else
    echo "Acceptable number of errors ($total_errors)."
fi
