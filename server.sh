#!/bin/bash
make clean
make ragel
make all
rm -rf run.log
./run -d 1
