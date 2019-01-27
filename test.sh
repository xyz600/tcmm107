#!/bin/bash

./build.sh

java PointsOnGridVis -exec "./a.out" -seed $1
