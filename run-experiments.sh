#!/bin/sh -eu


echo "Compiling stuff..."
make

for j in $(seq 1 50); do
    for i in $(seq 2 10 1000); do
        ./main --nodes="$i" --mode="$1"
    done
done

