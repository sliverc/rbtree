#!/bin/sh

TYPE="$1"

cd "$BUILD"
taskset -c 0 "./$TYPE" > log1
taskset -c 0 "./$TYPE" > log2
taskset -c 0 "./$TYPE" > log3
taskset -c 0 "./$TYPE" > log4
taskset -c 0 "./$TYPE" > log5
"$BASE/mk/avg" log1 log2 log3 log4 log5 > log
gnuplot -c "$BASE/mk/$TYPE" > "$BASE/$TYPE".png
