#!/bin/bash
make test
rm -fv test.log
touch test.log
./test -i 1
tail -7 test.log 
