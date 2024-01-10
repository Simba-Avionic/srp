cpp_files=$(find . -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp")
total_errors=0
for file in $cpp_files; do
    errors=$(cpplint --filter=-build/include_subdir --extensions=cc,cpp,h,hpp --linelength=120 --recursive --repository=. --root=src $file | wc -l)
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