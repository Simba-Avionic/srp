#!/bin/bash
bazel test  `bazel query 'kind(cc_.*, tests(//...))'`