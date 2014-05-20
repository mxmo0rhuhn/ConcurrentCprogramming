CFLAGS=-march=native -std=gnu99 -g -O2 -I./include -L./lib
LIBS=-lpthread -ltermpaper

SERVER_FILE=server.c
CLIENT_FILE=client.c
TEST_FILE=moduleTest/moduleTest.c

SERVER_OUT=run
CLIENT_OUT=client
TEST_OUT=test

all: test run 

clean:
	rm -fv lib/*.a 
	rm -fv lib/*.o 
	rm -fv $(CLIENT_OUT) 
	rm -fv $(SERVER_OUT) 
	rm -fv $(TEST_OUT) 

# the Server 
run: $(SERVER_FILE) lib/libtermpaper.a 
	gcc $(CFLAGS) $(SERVER_FILE) $(LIBS) -o $(SERVER_OUT)

# an interactive client
client: $(CLIENT_FILE) lib/libtermpaper.a 
	gcc $(CFLAGS) $(CLIENT_FILE) $(LIBS) -o $(CLIENT_OUT)

# some module tests for the framework
test: $(TEST_FILE) lib/libtermpaper.a 
	gcc $(CFLAGS) $(TEST_FILE) $(LIBS) -o $(TEST_OUT)

# shared libs
lib/termPaperLib.o: lib/termPaperLib.c include/termPaperLib.h
	gcc -c $(CFLAGS) lib/termPaperLib.c -o lib/termPaperLib.o

lib/messageProcessing.o: lib/messageProcessing.c include/messageProcessing.h
	gcc -c $(CFLAGS) lib/messageProcessing.c -o lib/messageProcessing.o

lib/concurrentLinkedList.o: lib/concurrentLinkedList.c lib/termPaperLib.o
	gcc -c $(CFLAGS) lib/concurrentLinkedList.c -o lib/concurrentLinkedList.o

lib/libtermpaper.a: lib/termPaperLib.o lib/concurrentLinkedList.o lib/messageProcessing.o
	ar crs lib/libtermpaper.a lib/termPaperLib.o lib/concurrentLinkedList.o lib/messageProcessing.o 

# ragel - special processing for input parsing
ragel: lib/messageProcessing.rl 
	ragel -C lib/messageProcessing.rl -o lib/messageProcessing.c
