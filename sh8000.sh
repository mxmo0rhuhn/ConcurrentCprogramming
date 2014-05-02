#!/bin/bash
make
echo "aa bb cc aa" > bla
./server.out 8000 -f bla
