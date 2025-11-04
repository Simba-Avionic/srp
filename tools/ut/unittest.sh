#!/bin/bash
bazel test  --nocache_test_results --test_output=all `bazel query 'kind(cc_test, //...)'`
