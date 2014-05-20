#!/bin/bash
make test
./test 127.0.0.1 -p 7000 > moduleTest/test.log
tail -3 moduleTest/test.log
