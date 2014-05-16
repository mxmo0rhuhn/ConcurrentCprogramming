#!/bin/bash
make clean
make ragel
make module_test
make
./server.out 7000 
