#!/bin/bash
make clean
make ragel
make module_test
make
./server.out 8000 
