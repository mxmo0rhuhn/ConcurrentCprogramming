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
  char *usage = "[-c Line1] [-C Line2]";
  char *ip_help = get_ip_help(&usage);
  char *port_help = get_port_help(&usage);
  char *log_help = get_logging_help(&usage);
  printf("Usage:\n");
  printf("%s %s\n\n", argv0, usage);

  printf("Connects to a server and provides an interactive multiline command\n");
  printf("interface. You can simply start a new line with \\n (Enter)\n");
  printf("The command will be send to the server as soon as a empty line is\n");
  printf("entered. The interactive interface can be ended using <Ctrl-C> \n");
  printf("or by entering QUIT\n\n");
  printf("You may also provide a command that is executed without an interactive\n");
  printf("command interface using the [-c Line] option\n");
  printf("If you want to specify a second line that is send to the server after\n");
  printf("-c you can use -C \n\n\n");

  printf("[-c Line1] Optional: A command that should be send to the server.\n");
  printf("                     This option disables the interactive mode\n\n");
  printf("[-C Line2] Optional: A second line for the command - only possible\n");
  printf("                     if a first line is provided\n\n");
  printf("%s\n", ip_help);
  printf("%s\n", port_help);
  printf("%s\n\n", log_help);

  printf("(c) Max Schrimpf - ZHAW 2014\n");
  exit(1);
}

int main(int argc, char *argv[]) {

  int retcode;
  if (is_help_requested(argc, argv)) {
    usage(argv[0], "Help:");
  }

  char *server_ip = get_ip_with_default(argc, argv);
  unsigned short server_port = get_port_with_default(argc, argv);
  get_logging_properties(argc, argv);

  int sock;                        
  int interactive = TRUE; 
  int firstRun = TRUE;
  char *cmd;
  int cmdLen;

  int i;
  for (i = 2; i < argc; i++)  {
    if (strcmp(argv[i], "-c") == 0)  {
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
