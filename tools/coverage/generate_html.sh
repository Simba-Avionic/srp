#!/bin/bash
export GCOV=/usr/bin/gcov-13
bazel coverage --nocache_test_results `bazel query 'kind(cc_test, //...)'`  -s \
  --instrument_test_targets \
  --experimental_cc_coverage \
  --combined_report=lcov \
  --instrumentation_filter="[:]"
  --coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main > /dev/null 2>&1

lcov  --summary bazel-out/_coverage/_coverage_report.dat
python3 tools/coverage/remove_external.py

genhtml bazel-out/_coverage/coverage_report.dat --output-directory coverage-report
