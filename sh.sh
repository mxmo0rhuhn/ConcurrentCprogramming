#!/bin/bash
make
echo "aa bb cc aa" > bla
./server.out 7000 -f faust.txt
