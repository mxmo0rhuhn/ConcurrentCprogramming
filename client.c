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

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [-c Command] [-p Port]\n", argv0);
  printf("Connects to the Server and provides an interactive multiline Commandline\n");
  printf("You may also provide a certain command that is to be executed with -c\n");
  printf("If you want to specify a second line that is send to the server after\n");
  printf("-c you can use -C \n");
  printf("If you want to specify a special port use -p \n");
  printf("End with Ctrl-C or QUIT\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;
  if (is_help_requested(argc, argv)) {
    usage(argv[0], "");
  }

  if (argc < 2 ) {  
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
    } else if (strcmp(argv[i], "-C") == 0)  {
      if (i + 2 <= argc )  {

        if(interactive == TRUE) {
          usage(argv[0], "please provide a first line withe -c if you're using -C");
        }

        i++;
        cmd = join_with_seperator(cmd, argv[i], "\n");
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
      send = join_with_seperator(cmd, "", "\n");
    } else {
      char line[2048];
      char c;
      char lastc = '\n';
      int i = 0;

      printf("CMD=");
      fflush(stdout);

      while (EOF!= ( c = getc(stdin))) {

        if (lastc == '\n' && c == '\n') {
          break;
        }
        lastc = c;
        line[i]  = c;
        i++;
      }
      line[i]  = '\000';

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
    log_debug("Sendling: '%s'\n", send);
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
