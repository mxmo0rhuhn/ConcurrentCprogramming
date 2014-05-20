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
  printf("Usage:\n");
  printf("%s <Server IP> [-c Line1] [-C Line2] [-p Port]\n", argv0);
  printf("<Server IP> Mandatory: Tries to connect to a server with the given IP\n");
  printf("[-c Line1] Optional: A command that should be send to the server.\n");
  printf("                     This option disables the interactive mode\n");
  printf("[-C Line2] Optional: A second line for the command - only possible\n");
  printf("                     if a first line is provided\n");
  printf("[-P Port] Optional: Tries to connect to a server on the given port.\n");
  printf("          Default: 7000\n\n");

  printf("Connects to a server and provides an interactive multiline command\n");
  printf("interface. You can simply start a new line with \\n (Enter)\n");
  printf("The command will be send to the server as soon as a empty line is\n");
  printf("entered. The interactive interface can be ended using <Ctrl-C> \n");
  printf("or by entering QUIT\n\n");
  printf("You may also provide a command that is executed without an interactive\n");
  printf("command interface using the [-c Line] option\n");
  printf("If you want to specify a second line that is send to the server after\n");
  printf("-c you can use -C \n\n");
  printf("(c) Max Schrimpf - ZHAW 2014\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;
  if (is_help_requested(argc, argv)) {
    usage(argv[0], "Help:");
  }

  if (argc < 2 ) {  
    usage(argv[0], "wrong number of arguments please provide an IP at least");
  }

  int sock;                        
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

    sock = create_client_socket(server_port, server_ip);

    log_debug("Sendling: '%s'\n", send);
    write_to_socket(sock, send);

    char *buffer_ptr[0];

    // Receive command from server 
    size_t received_msg_size = read_from_socket(sock, buffer_ptr);
    handle_error(received_msg_size, "recive failed", THREAD_EXIT);

    printf("Response=%s \n", *buffer_ptr);
    free(*buffer_ptr);

    close(sock);
    firstRun = FALSE;
  }
  exit(0);
}
