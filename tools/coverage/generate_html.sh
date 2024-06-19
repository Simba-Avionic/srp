#!/bin/bash
bazel coverage --combined_report=lcov  `bazel query 'kind(cc_.*, tests(//...))'`
genhtml bazel-out/_coverage/_coverage_report.dat --output-directory coverage-report