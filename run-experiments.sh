#!/bin/sh -eu


echo "Compiling stuff..."
make

for j in $(seq 1 50); do
    for i in $(seq 2 10 500); do
        ./build/synthetic-tests-simulator --nodes="$i" --graph-generator="$1" --aglorithm="$2"
    done
done

