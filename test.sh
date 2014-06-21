#!/bin/bash
make test
rm -fv test.log
touch test.log
./test 127.0.0.1 -p 7000 -i 1
tail -7 test.log 
