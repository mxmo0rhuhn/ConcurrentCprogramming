CFLAGS=-march=native -std=gnu99 -g -O2 -I./include -L./lib
LIBS=-lpthread -litsky -ltermpaper

SERVER_FILE=server.c
CLIENT_FILE=client.c

SERVER_OUT=server.out
CLIENT_OUT=client.out

all: uebung client 

clean:
	rm lib/*.a lib/*.o $(MYFILE) client.o

uebung: $(MYSOURCE) lib/libitsky.a lib/libtermpaper.a include/termPaperLib.h
	gcc $(CFLAGS) $(SERVER_FILE) $(LIBS) -o $(SERVER_OUT)

client: client.c lib/termPaperLib.o include/termPaperLib.h
	gcc $(CFLAGS) $(CLIENT_FILE) $(LIBS) -o $(CLIENT_OUT)

# Libs aus der Semesterarbeit

lib/termPaperLib.o: lib/termPaperLib.c
	gcc -c $(CFLAGS) lib/termPaperLib.c -o lib/termPaperLib.o

lib/concurrentLinkedList.o: lib/concurrentLinkedList.c lib/termPaperLib.o
	gcc -c $(CFLAGS) lib/concurrentLinkedList.c -o lib/concurrentLinkedList.o

lib/libtermpaper.a: lib/termPaperLib.o 
	ar crs lib/libtermpaper.a lib/termPaperLib.o 

#lib/termPaperLib.a: lib/termPaperLib.o lib/concurrentLinkedList.o
#	ar crs lib/termPaperLib.a lib/termPaperLib.o lib/concurrentLinkedList.o 

# Libs aus der Hausaufgabe

lib/fsort.o: lib/fsort.c
	gcc -c $(CFLAGS) lib/fsort.c -o lib/fsort.o

lib/fsort-metrics.o: lib/fsort-metrics.c
	gcc -c $(CFLAGS) lib/fsort-metrics.c -o lib/fsort-metrics.o

lib/isort.o: lib/isort.c
	gcc -c $(CFLAGS) lib/isort.c -o lib/isort.o

lib/ternary-hsort.o: lib/ternary-hsort.c
	gcc -c $(CFLAGS) lib/ternary-hsort.c -o lib/ternary-hsort.o

lib/msort.o: lib/msort.c
	gcc -c $(CFLAGS) lib/msort.c -o lib/msort.o

lib/transmission-protocols.o: lib/transmission-protocols.c
	gcc -c $(CFLAGS) lib/transmission-protocols.c -o lib/transmission-protocols.o

lib/psort.o: lib/psort.c
	gcc -c $(CFLAGS) lib/psort.c -o lib/psort.o

lib/hsort.o: lib/hsort.c
	gcc -c $(CFLAGS) lib/hsort.c -o lib/hsort.o

lib/sort_common.o: lib/sort_common.c
	gcc -c $(CFLAGS) lib/sort_common.c -o lib/sort_common.o

lib/lib.o: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o

lib/libitsky.a: lib/fsort.o lib/fsort-metrics.o lib/isort.o lib/ternary-hsort.o lib/msort.o lib/transmission-protocols.o lib/psort.o lib/hsort.o lib/sort_common.o
	ar crs lib/libitsky.a lib/fsort.o lib/fsort-metrics.o lib/isort.o lib/ternary-hsort.o lib/msort.o lib/transmission-protocols.o lib/psort.o lib/hsort.o lib/sort_common.o
