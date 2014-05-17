#!/bin/bash
make clean
make ragel
make all
./server.out 7000 
