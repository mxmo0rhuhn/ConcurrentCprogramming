#!/bin/bash
make clean
make ragel
make all
./run -p 7000 -d 1
