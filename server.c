/*
 * Uebung 12 - Systemprogrammierung
 * Max Schrimpf 
 * Hausaufgabe 7
 *
 * Server: 
 * - Erstellt einen Server, der auf eingehende connections wartet und diesen
 *   das angegebene File sortiert sendet.
 * - Bei der ersten Connection wird eine Threadsafe Funktion aufrufen, die: 
 *     - Die angegebene Datei liest
 *     - Die Zeichenkette der Datei extrahiert
 *     - Die Zeichenkette der Datei sortiert
 *     - Die Sortierte Zeichenkette in Cache schreibt
 * - Alle nachfolgenden Threads erhalten den gleichen Cache gesendet
 *
 * Basiert zum Teil auf den im GitHub angegebenen Beispielen
 */

/* enable qsort_r */
#define _GNU_SOURCE

// Shared Libs aus der Semesterarbeit
#include <termPaperLib.h>
#include <concurrentLinkedList.h>

// Shared libs aus Vorlesung
#include <hsort.h>
#include <fsort.h>
#include <isort.h>
#include <ternary-hsort.h>
#include <msort.h>
#include <fsort-metrics.h>
#include <psort.h>
#include <transmission-protocols.h>

// stdout 
#include <stdio.h>
//huebsche file fehler
#include <errno.h>
// string stuff
#include <string.h>
// threads
#include <pthread.h>
// exit
#include <stdlib.h>
// read 
#include <unistd.h>
// sockets & co
#include <sys/socket.h>
// addrtype 
#include <netdb.h>
// inet_ntoa
#include <arpa/inet.h>


// config für den Server
#define MAX_PENDING_CONNECTIONS 10
#define CMD_LEN 3
#define RCV_LEN 32
#define CMD "GET"
#define CLEANUP_FREQUENCY 10
#define RETURN_LINE_LEN 200


enum mt_sort_type {       
  MT_HEAP_SORT
    , MT_TERNARY_HEAP_SORT
    , MT_QUICK_SORT
    , MT_FLASH_SORT
    , MT_INSERTION_SORT
    , MT_MERGE_SORT 
};

// alle Informationen, die ein Thread benoetigt
typedef struct payload {
  pthread_t *thread;
  int socket;
  struct sockaddr_in client_address; 
  pthread_once_t *once;
} Payload;

// Globale Variablen
int port_number;

struct file {
  char *filename;
  struct string_array cache;
  enum mt_sort_type selected_sort_type;
} inFile;


void usage(char *programName, char *msg) {
  printf("%s\n\n", msg);
  printf("Usage:\n\n");

  printf("%s [port] -[sort protocoll] [file] \n", programName);
  printf("[port] starts a server listening on the given port\n");
  printf("[sort protocoll] sorts contents of file using given sort algorithm.\n"); 
  printf("f - flashsort\n");
  printf("h - heapsort\n");
  printf("t - ternary heapsort\n");
  printf("q - quicksort\n");
  printf("i - insertionsort\n");
  printf("m - mergesort\n");
  printf("[file] file that has to be sorted\n\n");

  printf("Homework for TCP Sockets\n");
  printf("Will start a server that returns the result of the given input file \n");
  printf("after it was split into words and sorted\n");
  printf("This is done only for the for the first client request\n");
  printf("and served to the client via TCP if he sends 'GET\\n'\n\n");

  printf("All characters other than [A-Za-z0-9\240-\377] will mark word boundaries\n");
  printf("Words longer than 200 characters exceed the width in a line by themselves\n");
  printf("Reading and sorting is only done once for the connection that comes first.\n");
  printf("Output is then rendered and transmitted by each connection separately\n");

  exit(1);
}

void init_function() {
  log_info("One time Funktion startet - this should never happen again");

  typedef char *char_ptr;
  FILE *file = fopen(inFile.filename, "r+");
  handle_ptr_error(file, "Could not open file", PROCESS_EXIT);

  inFile.cache = read_to_array(fileno(file));
  fclose(file);

  switch (inFile.selected_sort_type) {
    case MT_HEAP_SORT:
      hsort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL);
      break;
    case MT_TERNARY_HEAP_SORT:
      ternary_hsort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL);
      break;
    case MT_QUICK_SORT:
      qsort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL);
      break;
    case MT_FLASH_SORT:
      fsort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL
          , metric_str_full, (void *) NULL);
      break;
    case MT_INSERTION_SORT:
      isort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL);
      break;
    case MT_MERGE_SORT:
      msort_r(inFile.cache.strings, inFile.cache.len
          , sizeof(char_ptr), compare_str_full, (void *) NULL);
      break;
    default:
      /* should *never* happen: */
      handle_error_myerrno(-1, -1, "wrong mt_sort_type", PROCESS_EXIT);
  }

  log_info("One time Funktion finished");
}

void sendCache(int socket, long threadID) {
  char *buffer;
  int i = 0; 
  for( i = 0; i < inFile.cache.len; i++){
    log_info("Thread %ld: Sending %s", threadID, inFile.cache.strings[i]);
    write_string(socket, inFile.cache.strings[i], -1);
  }
}

void *handleRequest(void *input) {
  long threadID =(long) pthread_self();
  Payload *payload = ( Payload* ) input;

  log_info("Thread %ld: Hello - handling client %s", threadID
      , inet_ntoa(payload->client_address.sin_addr));

  char *buffer_ptr[0];

  /* Receive command from client */
  size_t received_msg_size = read_and_store_string(payload->socket, buffer_ptr);
  handle_error(received_msg_size, "recive failed", THREAD_EXIT);

  log_info("Thread %ld: Recived: '%s'", threadID, *buffer_ptr);

  if (received_msg_size == CMD_LEN) {
    if (strncmp(*buffer_ptr, CMD, CMD_LEN) != 0) {
      close(payload->socket);    
      handle_thread_error(-1, "CMD not known \n", THREAD_EXIT);
    }
    // wenn es hier vorbei kommt ist alles gut
    log_info("Thread %ld: I know what I shall do", threadID);
  } else {
    close(payload->socket);    
    handle_thread_error(-1, "CMD not known - length not known\n", THREAD_EXIT);
  }

  int retcode = pthread_once((payload->once), init_function);
  handle_thread_error(retcode, "call of once fkt", THREAD_EXIT);

  sendCache(payload->socket, threadID);

  /* Close client socket */
  close(payload->socket);    

  log_info("Thread %ld: Bye Bye", threadID );  
  return NULL;
}

void *createSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from Socket Listener", threadID );  

  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) input;
  Payload *nextListEntry = malloc(sizeof(Payload));
  pthread_once_t once = PTHREAD_ONCE_INIT;
  nextListEntry->once = &once;

  /* Create socket for incoming connections */
  int server_socket;                    
  struct sockaddr_in server_address; 
  unsigned int client_address_len = sizeof(nextListEntry->client_address);

  // Construct local address structure 
  memset(&server_address, 0, sizeof(server_address));   
  server_address.sin_family = AF_INET;               
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  server_address.sin_port = htons(port_number);      

  // Bind to the local address 
  server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);
  int retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  /* Mark the socket so it will listen for incoming connections */
  retcode = listen(server_socket, MAX_PENDING_CONNECTIONS);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  /* Run forever */
  while (TRUE) { 
    log_debug("Accept - Payload: %p", nextListEntry);
    nextListEntry->thread = malloc(sizeof(pthread_t));
    log_debug("allocated thread: %p", nextListEntry->thread);

    /* Wait for a client to connect */
    log_info("Socket Listener: Accepting new connections");
    nextListEntry->socket = accept(server_socket 
        , (struct sockaddr *)&(nextListEntry->client_address) 
        , &(client_address_len));
    handle_error(nextListEntry->socket, "accept() failed", PROCESS_EXIT);
    log_info("Socket Listener: New connection accepted");

    int retcode = pthread_create(nextListEntry->thread , NULL, handleRequest, nextListEntry);
    handle_thread_error(retcode, "Create Thread", PROCESS_EXIT);

    appendListElement(threadList, &nextListEntry, sizeof(Payload), "NoName");
  }
  // Should never happen!
  log_info("Thread %ld: Bye Bye from Socket Listener - ERROR!", threadID );  
  return (void *) -1;
}

void *cleanUpSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from Cleanup Thread", threadID );  

  int retcode;
  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) input;
  log_debug("ConcurrentLinkedList: %p", threadList);
  void *firstElementCopy;

  while (TRUE) { 
    sleep(CLEANUP_FREQUENCY);
    log_info("Cleanup Thread: Start Removing Threads");
    log_debug("First Element Copy = %p", firstElementCopy);

    // Gibt eine shallow copy des ersten Elementes zurück
    while(getFirstListElement(threadList, &firstElementCopy) > 1){ 
      log_info("Cleanup Thread: Removing Thread");

      Payload *firstElementAsPayload = (Payload *) firstElementCopy;

      log_debug("about to join: %p", firstElementAsPayload->thread);
      retcode = pthread_join(*(firstElementAsPayload->thread), NULL);
      handle_thread_error(retcode, "Join Thread", PROCESS_EXIT);
      log_debug("joined");

      log_debug("remove thread: %p", firstElementAsPayload->thread);
      free(firstElementAsPayload->thread);
      log_debug("about to remove copy: %p", firstElementCopy);
      free(firstElementCopy);

      removeFirstListElement(threadList);
    }
    log_info("Cleanup Thread: Done Removing Threads");
  }
  // Should never happen!
  log_error("Thread %ld: Bye Bye from Cleanup Thread - ERROR!", threadID );  
  return (void *) -1;
}

int main ( int argc, char *argv[] ) {

  char *programName = argv[0];
  if (argc != 4) {
    log_info("found %d arguments", argc - 1);
    usage(programName, "wrong number of arguments");
  }

  inFile.filename = argv[3];
  port_number = atoi(argv[1]); 
  char *argv_opt = argv[2];

  if (strlen(argv_opt) != 2 || argv_opt[0] != '-') {
    usage(programName, "wrong option");
  }

  char opt_char = argv_opt[1];
  switch (opt_char) {
    case 'h' :
      inFile.selected_sort_type = MT_HEAP_SORT;
      break;
    case 't' :
      inFile.selected_sort_type = MT_TERNARY_HEAP_SORT;
      break;
    case 'f' :
      inFile.selected_sort_type = MT_FLASH_SORT;
      break;
    case 'q' :
      inFile.selected_sort_type = MT_QUICK_SORT;
      break;
    case 'i' :
      inFile.selected_sort_type = MT_INSERTION_SORT;
      break;
    case 'm' :
      inFile.selected_sort_type = MT_MERGE_SORT;
      break;
    default:
      usage(programName, "wrong option");
      break;
  }

  // erstelle Start der linked List
  ConcurrentLinkedList *threadList = newList();
  log_debug("ConcurrentLinkedList: %p", threadList);

  pthread_t socketListenerThread;
  pthread_t cleanUpThread;

  int retcode = pthread_create(&socketListenerThread, NULL, createSocketListener, threadList); 
  handle_thread_error(retcode, "Create Thread", PROCESS_EXIT);

  // alles aufraeumen
  retcode = pthread_create(&cleanUpThread , NULL, cleanUpSocketListener,threadList);
  handle_thread_error(retcode, "Create Thread", PROCESS_EXIT);

  retcode = pthread_join( socketListenerThread, NULL);
  handle_thread_error(retcode, "Join Thread", PROCESS_EXIT);
  retcode = pthread_join( cleanUpThread, NULL);
  handle_thread_error(retcode, "Join Thread", PROCESS_EXIT);

  // wird nie passieren, da beide threads endlos laufen
  log_error("Master: Reached exit - this should never happen");
  exit(-1);
}
