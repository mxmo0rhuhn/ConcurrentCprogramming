#!/bin/bash
make clean
make ragel
make all
./run 7000 
