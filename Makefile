CFLAGS=-march=native -std=gnu99 -g -O2 -I./include -L./lib
LIBS=-lpthread -ltermpaper

SERVER_FILE=server.c
CLIENT_FILE=client.c

SERVER_OUT=server.out
CLIENT_OUT=client.out

all: run test 

clean:
	rm lib/*.a lib/*.o test/*.o $(CLIENT_OUT) $(SERVER_OUT) 

run: $(SERVER_FILE) lib/libtermpaper.a 
	gcc $(CFLAGS) $(SERVER_FILE) $(LIBS) -o $(SERVER_OUT)

test: $(CLIENT_FILE) lib/termPaperLib.o 
	gcc $(CFLAGS) $(CLIENT_FILE) $(LIBS) -o $(CLIENT_OUT)

# some module tests for the framework

module_test: linked_list_test

linked_list_test: test/linkedListTest.c lib/libtermpaper.a include/termPaperLib.h
	gcc $(CFLAGS) test/linkedListTest.c $(LIBS) -o test/linkedListTest.o

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