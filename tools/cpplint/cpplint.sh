cpp_files=$(find . -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp")
total_errors=0
for file in $cpp_files; do
    errors=$(cpplint --filter=-legal/copyright,-build/c++11,-build/include_subdir --extensions=cc,cpp,h,hpp --linelength=120 --recursive --repository=. --root=src $file | wc -l)
    total_errors=$((total_errors + errors))
done
          
if [[ $total_errors -gt 15 ]]; then
    echo "Too many errors ($total_errors), failing the build."
    exit 1
else
    echo "Acceptable number of errors ($total_errors)."
fi