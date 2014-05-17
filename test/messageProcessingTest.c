/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides a testsuite for the project
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

char *server_ip;
unsigned short server_port;
int num_testcases;
int num_testcases_success;
int num_testcases_fail;

void runTestcase(char *input, char *expected) {
  num_testcases++;

  struct sockaddr_in server_address; 
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(sock, "socket() failed", PROCESS_EXIT);

  memset(&server_address, 0, sizeof(server_address));     
  server_address.sin_family      = AF_INET;             
  server_address.sin_addr.s_addr = inet_addr(server_ip);   
  server_address.sin_port        = htons(server_port); 

  int retcode = connect(sock, (struct sockaddr *) &server_address, 
      sizeof(server_address));
  handle_error(retcode, "connect() failed\n", PROCESS_EXIT);

//  log_debug("Sendling: '%s'\n", input);
  write_string(sock, input, -1);

  sleep(1);
  char *buffer_ptr[0];

  size_t received_msg_size = read_and_store_string(sock, buffer_ptr);
  handle_error(received_msg_size, "recive failed", THREAD_EXIT);

//  log_debug("Response=%s \n", *buffer_ptr);
  int result = strcmp(*buffer_ptr, expected);

  if(result == 0) {
    log_info("Testcase %zu: OK!", num_testcases);
    num_testcases_success++;  
  } else {
    log_error("Testcase %zu: FAILED!", num_testcases);
    log_error("send: '%s'", input);
    log_error("Expected: '%s'", expected);
    log_error("Recived : '%s'", *buffer_ptr);
    
    num_testcases_fail++;  
  }
  free(*buffer_ptr);

  close(sock);
}

void runTestcases() {
//filename with spaces 
  runTestcase("CREATE im possible 3\n123\n", "COMMAND_UNKNOWN\n");
  runTestcase("UPDATE im possible 3\n123\n", "COMMAND_UNKNOWN\n");
  runTestcase("DELETE im possible 3\n123\n", "COMMAND_UNKNOWN\n");
  runTestcase("READ im possible 3\n123\n", "COMMAND_UNKNOWN\n");

  runTestcase("DELETE lol\n", "NOSUCHFILE\n");
  runTestcase("READ lol\n", "NOSUCHFILE\n");
  runTestcase("LIST\n", "ACK 0\n");
  runTestcase("CREATE lol 3\n123\n", "FILECREATED\n");
  runTestcase("CREATE lol 3\n472\n", "FILEEXISTS\n");
  runTestcase("READ lol\n", "FILECONTENT lol 3\n123\n");
  runTestcase("LIST\n", "ACK 1\nlol\n");
  runTestcase("DELETE lol\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 0\n");
  runTestcase("READ lol\n", "NOSUCHFILE\n");
  runTestcase("DELETE lol\n", "NOSUCHFILE\n");
  runTestcase("CREATE lol 3\n123\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 1\nlol\n");
  runTestcase("READ lol\n", "FILECONTENT lol 3\n123\n");
  runTestcase("CREATE rofl 7\nasdifgj\n", "FILECREATED\n");
  runTestcase("READ lol\n", "FILECONTENT lol 3\n123\n");
  runTestcase("READ rofl\n", "FILECONTENT rofl 7\nasdifgj\n");
  runTestcase("LIST\n", "ACK 2\nlol\nrofl\n");
  runTestcase("CREATE hack1 0\nasdfgh\n", "COMMAND_UNKNOWN\n");
  runTestcase("LIST\n", "ACK 2\nlol\nrofl\n");
  runTestcase("CREATE hack1 1\nasdfgh\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 3\nlol\nrofl\nhack1\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\na\n");
  runTestcase("DELETE hack1\n", "DELETED\n");
  runTestcase("DELETE hack1\n", "NOSUCHFILE\n");
  runTestcase("READ hack1\n", "NOSUCHFILE\n");
  runTestcase("CREATE hack1 1\nasdfgh\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 3\nlol\nrofl\nhack1\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\na\n");
  runTestcase("CREATE hack3 0\n\n", "COMMAND_UNKNOWN\n");
  runTestcase("LIST\n", "ACK 3\nlol\nrofl\nhack1\n");
  runTestcase("CREATE hack3 3\n1\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 4\nlol\nrofl\nhack1\nhack3\n");
  runTestcase("READ hack3\n", "FILECONTENT hack3 1\n1\n");
  runTestcase("CREATE hack2 1337\n\n", "COMMAND_UNKNOWN\n");
  runTestcase("READ hack2\n", "NOSUCHFILE\n");
  runTestcase("CREATE hack2 1337\n1\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 5\nlol\nrofl\nhack1\nhack3\nhack2\n");
  runTestcase("READ hack2\n", "FILECONTENT hack2 1\n1\n");
  runTestcase("CREATE hack4 1337\nqqqqqqqq\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 6\nlol\nrofl\nhack1\nhack3\nhack2\nhack4\n");
  runTestcase("READ hack4\n", "FILECONTENT hack4 8\nqqqqqqqq\n");
  runTestcase("DELETE hack4\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 5\nlol\nrofl\nhack1\nhack3\nhack2\n");
  runTestcase("DELETE hack4\n", "NOSUCHFILE\n");
  runTestcase("LIST\n", "ACK 5\nlol\nrofl\nhack1\nhack3\nhack2\n");
  runTestcase("CREATE anotherFile 10\n12erhl7d9k\n", "FILECREATED\n");
  runTestcase("LIST\n", "ACK 6\nlol\nrofl\nhack1\nhack3\nhack2\nanotherFile\n");
  runTestcase("READ anotherFile\n", "FILECONTENT anotherFile 8\n12erhl7d9k\n");
  runTestcase("DELETE lol\n", "DELETED\n");
  runTestcase("DELETE lol\n", "NOSUCHFILE\n");
  runTestcase("READ rofl\n", "FILECONTENT rofl 7\nasdifgj\n");
  runTestcase("LIST\n", "ACK 5\nrofl\nhack1\nhack3\nhack2\nanotherFile\n");
  runTestcase("DELETE hack3\n", "DELETED\n");
  runTestcase("DELETE hack3\n", "NOSUCHFILE\n");
  runTestcase("LIST\n", "ACK 4\nrofl\nhack1\nhack2\nanotherFile\n");
  runTestcase("READ anotherFile\n", "FILECONTENT anotherFile 8\n12erhl7d9k\n");
  runTestcase("READ rofl\n", "FILECONTENT rofl 7\nasdifgj\n");
  runTestcase("READ hack4\n", "NOSUCHFILE\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\na\n");
  runTestcase("READ hack2\n", "FILECONTENT hack2 1\n1\n");

// UPDATE TESTS
  runTestcase("UPDATE hack1 0\nasdfgh\n", "COMMAND_UNKNOWN\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\na\n");
  runTestcase("UPDATE hack1 1\nzzzzzzzz\n", "UPDATED\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\nz\n");
  runTestcase("UPDATE hack1 13371\n\n", "COMMAND_UNKNOWN\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 1\nz\n");
  runTestcase("UPDATE hack1 13371\nxy\n", "UPDATED\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 2\nxy\n");
  runTestcase("UPDATE hack1 3\nlkjh\n", "UPDATED\n");
  runTestcase("READ hack1\n", "FILECONTENT hack1 3\nlkj\n");

//content with spaces 
  runTestcase("CREATE withSpaces 3\ni i\n", "FILECREATED\n");
  runTestcase("READ withSpaces\n", "FILECONTENT withSpaces 3\ni i\n");
  runTestcase("DELETE withSpaces\n", "DELETED\n");

  runTestcase("READ rofl\n", "FILECONTENT rofl 7\nasdifgj\n");
  runTestcase("READ hack4\n", "NOSUCHFILE\n");
  runTestcase("READ hack2\n", "FILECONTENT hack2 1\n1\n");
  runTestcase("LIST\n", "ACK 4\nrofl\nhack1\nhack2\nanotherFile\n");
  runTestcase("DELETE anotherFile\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 3\nrofl\nhack1\nhack2\n");
  runTestcase("DELETE hack1\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 2\nrofl\nhack2\n");
  runTestcase("UPDATE hack1 3\nlkjh\n", "NOSUCHFILE\n");
  runTestcase("DELETE rofl\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 1\nhack2\n");
  runTestcase("DELETE hack2\n", "DELETED\n");
  runTestcase("LIST\n", "ACK 0\n");
}

void usage(const char *argv0, const char *msg) {
  if (msg != NULL && strlen(msg) > 0) {
    printf("%s\n\n", msg);
  }
  printf("Usage\n\n");
  printf("%s <Server IP> [-p Port]\n", argv0);
  printf("Executes various tests on the fileserver\n");
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

  server_port = 7000;  
  server_ip = argv[1];             
  num_testcases = 0;
  num_testcases_success = 0;
  num_testcases_fail = 0;

  int i;
  for (i = 2; i < argc; i++)  {
    if (strcmp(argv[i], "-p") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        server_port = atoi(argv[i]);  
      } else {
        usage(argv[0], "please provide a port if you're using -p");
      }
    } else {
      usage(argv[0], "unknown input");
    }
  }

  runTestcases();

  printf("Testsuite done!\n");
  printf("Ran %d Tests\n", num_testcases);
  printf("%d Tests succeded\n", num_testcases_success);
  printf("%d Tests failed\n", num_testcases_fail);
}
