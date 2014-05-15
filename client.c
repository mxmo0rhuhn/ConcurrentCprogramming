/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides a client to access the server and perform requests
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

#include <arpa/inet.h>  
#include <stdio.h>
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     

#include <termPaperLib.h>
#include <transmission-protocols.h>

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [-c Command] [-p Port]\n", argv0);
  printf("Connects to the Server and provides an interactive Commandline\n");
  printf("You may also provide a certain command that is to be executed\n");
  printf("End with Ctrl-C or QUIT\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;
  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 2 || argc > 6) {    
    usage(argv[0], "wrong number of arguments please provide an IP at least");
  }

  int sock;                        
  struct sockaddr_in server_address; 
  unsigned short server_port = 7000;  
  char *server_ip = argv[1];             
  int interactive = TRUE; 
  int firstRun = TRUE;
  char *cmd;
  int cmdLen;

  int i;
  for (i = 2; i < argc; i++)  {
    if (strcmp(argv[i], "-p") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        server_port = atoi(argv[i]);  
      } else {
        usage(argv[0], "please provide a port if you're using -p");
      }
    } else if (strcmp(argv[i], "-c") == 0)  {
      if (i + 2 <= argc )  {

        interactive = FALSE;
        i++;
        cmd = argv[i];
        cmdLen = strlen(cmd);
      } else {
        usage(argv[0], "please provide a CMD if you're using -c");
      }
    } else {
      usage(argv[0], "unknown input");
    }
  }

  while (firstRun || interactive ) {

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    handle_error(sock, "socket() failed", PROCESS_EXIT);

    memset(&server_address, 0, sizeof(server_address));     
    server_address.sin_family      = AF_INET;             
    server_address.sin_addr.s_addr = inet_addr(server_ip);   
    server_address.sin_port        = htons(server_port); 

    retcode = connect(sock, (struct sockaddr *) &server_address, 
                             sizeof(server_address));
    handle_error(retcode, "connect() failed\n", PROCESS_EXIT);

    char *send;
    if (interactive  != TRUE){
      send = cmd;
    } else {
      char line[40];
      printf("CMD=");
      fflush(stdout);
      fgets(line, 40, stdin);
      int n = strlen(line);

      if (n == 0) {
        continue;
      }
      if (strcmp(line, "QUIT\n") == 0) {
        printf("terminated by user\n");
        break;
      }
      
      send = line;
    }
    printf("Sendling: '%s'\n", send);
    write_string(sock, send, -1);

    char *buffer_ptr[0];

    // Receive command from server 
    size_t received_msg_size = read_and_store_string(sock, buffer_ptr);
    handle_error(received_msg_size, "recive failed", THREAD_EXIT);

    printf("Response=%s \n", *buffer_ptr);
    free(*buffer_ptr);

    close(sock);
    firstRun = FALSE;
  }
  sleep(1);
  exit(0);
}
