#!/bin/sh

echo start

for i in {0..99}
    do
    echo $i
    ./testSpikingSudokuSolver_single $i
    done
