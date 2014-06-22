// TODO Beautification
// TODO join_with_seperator memory leak?

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

  char *usage =  "";
  char *port_help = get_port_help(&usage);
  char *log_help = get_logging_help(&usage);
  printf("%s %s\n\n", programName, usage);

  printf("Server for the term paper in concurrent C programming\n");
  printf("Will start a virtual file server that accepts connections via\n");
  printf("TCP\n\n\n");

  printf("%s\n", port_help);
  printf("%s\n\n", log_help);

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
  size_t received_msg_size = read_from_socket(payload->socket, buffer_ptr);
  log_debug("Thread %ld: Recived: '%s'", threadID, *buffer_ptr);

  char *return_msg = handle_message(received_msg_size, *buffer_ptr, payload->file_list);

  log_info("Thread %ld: Responding: '%s'", threadID, return_msg);
  write_to_socket(payload->socket, return_msg);

  // Close client socket 
  close(payload->socket);    
  free(*buffer_ptr);

  log_debug("Thread %ld: Bye Bye", threadID );  
  free(payload);
  pthread_exit(NULL);
}

void *createSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from LISTENER", threadID );  

  ListenerPayload *listenerPayload = (ListenerPayload *) input;
  Payload *nextListEntry = malloc(sizeof(Payload));
  pthread_once_t once = PTHREAD_ONCE_INIT;

  int server_socket = create_server_socket(listenerPayload->port_number);
  unsigned int client_address_len = sizeof(nextListEntry->client_address);

  int retcode;
  pthread_t *thread= malloc(sizeof(pthread_t));
  // Run forever 
  while (TRUE) { 
    log_debug("LISTENER: Accept - payload: %p", nextListEntry);
    log_debug("LISTENER: Allocated thread: %p", thread);

    nextListEntry->file_list = listenerPayload->file_list;

    // Wait for a client to connect 
    log_info("LISTENER: Accepting new connections");
    nextListEntry->socket = accept(server_socket , (struct sockaddr *)&(nextListEntry->client_address) , &(client_address_len));
    handle_error(nextListEntry->socket, "accept() failed", PROCESS_EXIT);

    log_info("LISTENER: New connection accepted");

    retcode = pthread_create(thread , NULL, handleRequest, nextListEntry);
    handle_thread_error(retcode, "Create Thread", PROCESS_EXIT);

    appendListElement(listenerPayload->threadList,(void *) thread, sizeof(pthread_t), "NoName");
    free(thread);
    nextListEntry = malloc(sizeof(Payload));
    thread= malloc(sizeof(pthread_t));
  }
  close(server_socket);
  // Should never happen!
  log_error("Thread %ld: Bye Bye from LISTENER - ERROR!", threadID );  
  pthread_exit(NULL);
}

void *cleanUpSocketListener(void *input) {
  long threadID =(long) pthread_self();
  log_info("Thread %ld: Hello from CLEANUP", threadID );  

  int retcode;
  ConcurrentLinkedList *threadList = (ConcurrentLinkedList *) input;
  log_debug("CLEANUP: ConcurrentLinkedList: %p", threadList);
  pthread_t *thread; 

  int num = 0;
  while (TRUE) { 
    sleep(CLEANUP_FREQUENCY);
    log_info("CLEANUP: Start removing threads");

    num = 0;
    // gives back the shallow copy of the element
    while(getFirstListElement(threadList, (void *)&thread) > 0){ 

      log_debug("CLEANUP: about to join: %p", thread);
      retcode = pthread_join(*thread, NULL);
      handle_thread_error(retcode, "Join thread", PROCESS_EXIT);
      log_debug("CLEANUP: joined");

      log_debug("CLEANUP: remove thread: %p", thread);
      free(thread);

      removeFirstListElement(threadList);
      num++;
    }
    log_info("CLEANUP: Removed %d threads", num);
  }
  // Should never happen!
  log_error("Thread %ld: Bye Bye from CLEANUP - ERROR!", threadID );  
  pthread_exit(NULL);
}

int main ( int argc, char *argv[] ) {
  char *programName = argv[0];

  if(is_help_requested( argc, argv)) {
    usage(programName, "Help: ");
  }

  get_logging_properties(argc, argv);

  // Creation of the file list
  ConcurrentLinkedList *file_list = newList();
  log_debug("MAIN: Server file_list: %p", file_list);

  // Creation of the active thread List
  ConcurrentLinkedList *threadList = newList();
  pthread_t socketListenerThread;

  ListenerPayload socketListenerPayload;
  socketListenerPayload.port_number = get_port_with_default(argc, argv);
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
  log_error("MAIN: Reached exit - this should never happen - ERROR");
  exit(-1);
}
