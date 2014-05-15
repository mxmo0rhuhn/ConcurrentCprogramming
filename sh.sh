#!/bin/bash
make clean
make ragel
make
./server.out 7000 
