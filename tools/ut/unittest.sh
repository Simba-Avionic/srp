#!/bin/bash

CACHE_FLAG=""

if [[ "$1" == "--no_cache" ]]; then
    CACHE_FLAG="--nocache_test_results"
fi

bazel test $CACHE_FLAG --test_output=all `bazel query 'kind(cc_test, //...)'`
