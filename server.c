// TODO Concurrency tests
// TODO Logging Config
//      - Central method and help text generator
// TODO Diffrent Locks for Content and everything else
// TODO Beautification

/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides the server logic for acceping and handling connections.
 * Copyright (C) 2014 Max Schrimpf
 *
 * The file is free software: You can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * The file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the project. if not, write to the Free Software Foundation, Inc.
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <termPaperLib.h>
#include <concurrentLinkedList.h>
#include <messageProcessing.h>

// all informations that are needed to handle requests
typedef struct payload {
  pthread_t *thread;
  int socket;
  struct sockaddr_in client_address; 
  ConcurrentLinkedList *file_list;
} Payload;

typedef struct listenerPayload {
  int port_number;
  ConcurrentLinkedList *threadList;
  ConcurrentLinkedList *file_list;
} ListenerPayload;

void usage(char *programName, char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage:\n");

  printf("%s <port> \n", programName);
  printf("<port> Mandatory: Starts the server listening on the given port\n\n");

  printf("Server for the term paper in concurrent C programming\n");
  printf("Will start a virtual file server that accepts connections via\n");
  printf("TCP\n\n");
  printf("(c) Max Schrimpf - ZHAW 2014\n");

  exit(1);
}

void *handleRequest(void *input) {
  long threadID =(long) pthread_self();
  Payload *payload = ( Payload* ) input;

  log_debug("Thread %ld: Hello - handling client %s", threadID
      , inet_ntoa(payload->client_address.sin_addr));

  char *buffer_ptr[0];

  // Receive command from client 
  size_t received_msg_size = read_and_store_string(payload->socket, buffer_ptr);
  handle_error(received_msg_size, "recive failed", THREAD_EXIT);
  log_debug("Thread %ld: Recived: '%s'", threadID, *buffer_ptr);

  char *return_msg = handle_message(received_msg_size, *buffer_ptr, payload->file_list);

  log_info("Thread %ld: Responding: '%s'", threadID, return_msg);
  write_string(payload->socket, return_msg, -1);

  // Close client socket 
  close(payload->socket);    
  free(*buffer_ptr);

  log_debug("Thread %ld: Bye Bye", threadID );  
  return NULL;
}

void *createSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from Socket Listener", threadID );  

  ListenerPayload *listenerPayload = (ListenerPayload *) input;
  Payload *nextListEntry = malloc(sizeof(Payload));
  pthread_once_t once = PTHREAD_ONCE_INIT;

  // Create socket for incoming connections 
  int server_socket;                    
  struct sockaddr_in server_address; 
  unsigned int client_address_len = sizeof(nextListEntry->client_address);

  // Construct local address structure 
  memset(&server_address, 0, sizeof(server_address));   
  server_address.sin_family = AF_INET;               
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  server_address.sin_port = htons(listenerPayload->port_number);      

  // Bind to the local address 
  server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);
  int retcode = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  // Mark the socket so it will listen for incoming connections 
  retcode = listen(server_socket, MAX_PENDING_CONNECTIONS);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  // Run forever 
  while (TRUE) { 
    log_debug("Accept - Payload: %p", nextListEntry);
    nextListEntry->thread = malloc(sizeof(pthread_t));
    log_debug("allocated thread: %p", nextListEntry->thread);
    nextListEntry->file_list = listenerPayload->file_list;

    // Wait for a client to connect 
    log_info("Socket Listener: Accepting new connections");
    nextListEntry->socket = accept(server_socket 
        , (struct sockaddr *)&(nextListEntry->client_address) 
        , &(client_address_len));
    handle_error(nextListEntry->socket, "accept() failed", PROCESS_EXIT);
    log_info("Socket Listener: New connection accepted");

    int retcode = pthread_create(nextListEntry->thread , NULL, handleRequest, nextListEntry);
    handle_thread_error(retcode, "Create Thread", PROCESS_EXIT);

    appendListElement(listenerPayload->threadList,(void *) &nextListEntry, sizeof(Payload), "NoName");
  }
  // Should never happen!
  log_error("Thread %ld: Bye Bye from Socket Listener - ERROR!", threadID );  
  return (void *) -1;
}

void *cleanUpSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from Cleanup Thread", threadID );  

  int retcode;
  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) input;
  log_debug("ConcurrentLinkedList: %p", threadList);
  Payload *firstElementAsPayload; 

  int num = 0;
  while (TRUE) { 
    sleep(CLEANUP_FREQUENCY);
    log_info("Cleanup Thread: Start Removing Threads");
    log_debug("First Element Copy = %p", firstElementAsPayload);

    num = 0;
    // gives back the shallow copy of the element
    while(getFirstListElement(threadList, (void *)&firstElementAsPayload) > 1){ 
      log_debug("Cleanup Thread: Removing Thread");

      log_debug("about to join: %p", firstElementAsPayload->thread);
      retcode = pthread_join(*(firstElementAsPayload->thread), NULL);
      handle_thread_error(retcode, "Join Thread", PROCESS_EXIT);
      log_debug("joined");

      log_debug("remove thread: %p", firstElementAsPayload->thread);
      free(firstElementAsPayload->thread);
      log_debug("about to remove copy: %p", firstElementAsPayload);
      free(firstElementAsPayload);

      removeFirstListElement(threadList);
      num++;
    }
    log_info("Cleanup Thread: Done Removing Threads");
    log_info("Removed %d Threads", num);
  }
  // Should never happen!
  log_error("Thread %ld: Bye Bye from Cleanup Thread - ERROR!", threadID );  
  return (void *) -1;
}

int main ( int argc, char *argv[] ) {
  char *programName = argv[0];

  if(is_help_requested( argc, argv)) {
    usage(programName, "Help: ");
  }

  if (argc != 2) {
    log_info("found %d arguments", argc - 1);
    usage(programName, "wrong number of arguments");
  }
  // Creation of the file list
  ConcurrentLinkedList *file_list = newList();
  log_debug("server file_list: %p", file_list);

  // Creation of the active thread List
  ConcurrentLinkedList *threadList = newList();
  pthread_t socketListenerThread;

  ListenerPayload socketListenerPayload;
  socketListenerPayload.port_number = atoi(argv[1]); 
  socketListenerPayload.threadList = threadList;
  socketListenerPayload.file_list = file_list;

  pthread_t cleanUpThread;

  int retcode = pthread_create(&socketListenerThread, NULL, 
                                createSocketListener, &socketListenerPayload); 
  handle_thread_error(retcode, "Create Listener thread", PROCESS_EXIT);

  // the cleanup should never happen since the threads should not finish
  retcode = pthread_create(&cleanUpThread , NULL, cleanUpSocketListener,threadList);
  handle_thread_error(retcode, "Create Cleanup thread", PROCESS_EXIT);

  retcode = pthread_join( socketListenerThread, NULL);
  handle_thread_error(retcode, "Join Listener thread", PROCESS_EXIT);
  retcode = pthread_join( cleanUpThread, NULL);
  handle_thread_error(retcode, "Join Cleanup thread", PROCESS_EXIT);

  // something went wrong!
  log_error("Master: Reached exit - this should never happen - ERROR");
  exit(-1);
}
