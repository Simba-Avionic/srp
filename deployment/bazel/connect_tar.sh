#!/bin/bash

tar -cf $1 -T /dev/null

for i in "${@:2}"; do
    tar --concatenate --file=$1 $i
done
