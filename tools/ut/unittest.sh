#!/bin/bash
bazel test  --nocache_test_results `bazel query 'kind(cc_test, //...)'`
