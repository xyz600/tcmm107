#!/bin/bash

./build.sh

num_testcase=2000

if [ $# -eq 1 ]; then
    num_testcase=$1
fi

if [ ! -e result ]; then
    mkdir result
fi
rm -r result/*

parallel --progress --result result java PointsOnGridVis -exec "./a.out" -novis -seed $1 ::: `seq 1 $num_testcase`
