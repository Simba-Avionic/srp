#!/bin/bash

if ! command -v bazel &> /dev/null; then
    echo "Bazel is not installed"
    exit 1
fi


if [[ "$1" == "--no_cache" ]]; then
    bazel clean --async
fi

bazel build //... -- -//deployment/tools/... -//tools/...