#!/bin/bash

# Uruchom Pylinta dla wszystkich plików .py i zapisz wynik w zmiennej
output=$(find . -name "*.py" | xargs pylint 2>&1)

# Wyodrębnij ocenę końcową z wyjścia
score=$(echo "$output" | grep "Your code has been rated at" | tail -1 | awk '{print $7}' | cut -d'/' -f1 | cut -d'.' -f1)

# Sprawdź, czy wynik jest poniżej 7
if [ "$score" -lt 7 ]; then
    echo "Error: Code rating is below 7. Here is the full output:"
    echo "$output"
else
    echo "Success: Code rating is $score/10"
fi
