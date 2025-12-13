#!/bin/bash

if [[ "$1" == "--no_cache" ]]; then
    bazel clean --async
fi

bazel build //... -- -//deployment/tools/... -//tools/...