#!/bin/bash
make module_test
test/linkedListTest.o
test/messageProcessingTest.o 127.0.0.1 -p 7000 > test/test.log
tail -3 test/test.log
