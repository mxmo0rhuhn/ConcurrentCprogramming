/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides library functions for common tasks and is based on teaching
 * material by Karl Brodowsky that was used for the system programming course
 * at Zurich University of Applied Sciences 2014 
 *
 * The file is free software: You can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * The file is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the project. if not, write to the Free Software Foundation, Inc.  51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <errno.h> 
#include <fcntl.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() and inet_ntoa() */ 
#include <unistd.h> 
#include <stdarg.h>

#include <termPaperLib.h>

void log_info(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char logLine[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("INFO: %s\n", logLine);
}

void log_debug(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char logLine[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("DEBUG: %s\n", logLine);
}

void log_error(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char logLine[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("ERROR: %s\n", logLine);
}

void exit_by_type(enum exit_type et) {
  switch (et) {
    case PROCESS_EXIT: 
      exit(1);
      break;
    case THREAD_EXIT:
      pthread_exit(NULL);
      break;
    case NO_EXIT:
      log_info("continuing");
      break;
    default:
      log_error("unknown exit_type=%d", et);
      exit(2);
      break;
  }
}

/* 
 * helper function for dealing with errors 
 */
void handle_error_myerrno(long return_code, int myerrno, 
    const char *msg, enum exit_type et) {
  if (return_code < 0) {
    const char *error_str = strerror(myerrno);

    if (msg != NULL) {
      log_error(msg);
    } 
    log_error("return_code=%ld\nerrno=%d\nmessage=%s\n", 
        return_code, myerrno, error_str);

    exit_by_type(et);
  }
}

void handle_thread_error(int retcode, const char *msg, enum exit_type et) {
  if (retcode != 0) {
    handle_error_myerrno(-abs(retcode), retcode, msg, et);
  }
}

/* 
 * helper function for dealing with errors 
 */
void handle_error(long return_code, const char *msg, enum exit_type et) {
  int myerrno = errno;
  handle_error_myerrno(return_code, myerrno, msg, et);
}

/* 
 * create a given file if it is not there 
 */
int create_if_missing(const char *pathname, mode_t mode) {
  int fd = creat(pathname, mode);
  if (fd < 0) {
    char s[1024];
    sprintf(s, "could not create file=\"%s\".", pathname);
    handle_error(fd, s, NO_EXIT);
    return fd;
  }
  int retcode = close(fd);
  handle_error(fd, "close", NO_EXIT);
  return retcode;
}

/* 
 * check if --help or similar is indicated 
 */
int is_help_requested(int argc, char *argv[]) {
  return (argc >= 2 
      && (strcmp(argv[1], "-h") == 0 
        || strcmp(argv[1], "-H") == 0
        || strcmp(argv[1], "-help") == 0
        || strcmp(argv[1], "--help") == 0));
}

int create_client_socket(int server_port, char *server_ip) {
  struct sockaddr_in server_address; 

  unsigned int address_len = sizeof(server_address);

  int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(client_socket, "socket() failed", PROCESS_EXIT);

  memset(&server_address, 0, address_len);
  server_address.sin_family = AF_INET;             
  server_address.sin_addr.s_addr = inet_addr(server_ip);   
  server_address.sin_port = htons(server_port); 

  int retcode = connect(client_socket, (struct sockaddr *) &server_address, address_len);
  handle_error(retcode, "connect() failed\n", PROCESS_EXIT);

  return client_socket;
}

int create_server_socket(int port_number) {
  // Create socket for incoming connections 
  int server_socket;                    
  struct sockaddr_in server_address; 
  unsigned int address_len = sizeof(server_address);

  // Construct local address structure 
  memset(&server_address, 0, sizeof(server_address));   
  server_address.sin_family = AF_INET;               
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); 
  server_address.sin_port = htons(port_number);      

  // Bind to the local address 
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  handle_error(server_socket, "socket() failed", PROCESS_EXIT);

  int retcode = bind(server_socket, (struct sockaddr *) &server_address, address_len);
  handle_error(retcode, "bind() failed", PROCESS_EXIT);

  // Mark the socket so it will listen for incoming connections 
  retcode = listen(server_socket, MAX_PENDING_CONNECTIONS);
  handle_error(retcode, "listen() failed", PROCESS_EXIT);

  return server_socket;
}

size_t read_from_socket(int client_socket, char **result) {

  log_debug("read_and_store_string client_socket = %d",client_socket);
  // CONTENT + FILENAME + other stuff 
  size_t message_max_len = MAX_MSG_LEN;
  char buffer[message_max_len+1];
  size_t bytes_received = 0;

  /* Receive up to the read_len bytes from the sender */
  bytes_received = read(client_socket, buffer, message_max_len);
  if (bytes_received <= 0) {
    log_error("recv() failed or connection closed prematurely");
    close(client_socket);
    exit_by_type(THREAD_EXIT);
  }
  //bad people may send strings that are not \000 terminated
  buffer[bytes_received] = '\000';

  *result = (char *) malloc(bytes_received+1);
  strncpy(*result, buffer, bytes_received+1);

  return bytes_received ;
}

void write_to_socket(int client_socket, const char *str) {
  int len = strlen(str);

  log_debug("write_string client_socket = %d",client_socket);
  size_t partial_len = write(client_socket, str, len);
  if (partial_len != len) {
    log_error("Send message to client failed");
    close(client_socket);
    exit_by_type(THREAD_EXIT);
  }
}
  
char *join_with_seperator(const char *str1, const char *str2, const char *sep) {

  size_t str1_len = strlen(str1);
  size_t str2_len = strlen(str2);
  size_t sep_len = strlen(sep);

  size_t tmp_len = str1_len + str2_len + sep_len + 1;

  // number of bytes including \000
  char *tmp = (char *) malloc(sizeof(unsigned char) * tmp_len);
  tmp[0] = '\000';

  snprintf(tmp, tmp_len, "%s%s%s", str1, sep, str2);

  return tmp;
}
