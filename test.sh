#!/bin/bash

./build.sh

java -jar tester.jar -exec "./a.out" -seed $1
