#!/bin/bash

if ! command -v bazel &> /dev/null; then
    echo "Bazel is not installed"
    exit 1
fi


CACHE_FLAG=""

if [[ "$1" == "--no_cache" ]]; then
    CACHE_FLAG="--nocache_test_results"
fi

bazel test $CACHE_FLAG --test_output=all `bazel query 'kind(cc_test, //...)'`
