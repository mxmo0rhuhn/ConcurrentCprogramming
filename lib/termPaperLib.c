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
  char logLine[1024];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("INFO: %s\n", logLine);
}

void log_debug(const char *msg, ...) {
  char logLine[1024];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("DEBUG: %s\n", logLine);
}

void log_error(const char *msg, ...) {
  char logLine[1024];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("ERROR: %s\n", logLine);
}

void exit_by_type(enum exit_type et) {
  char error_msg[1024] ; 
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
      sprintf(error_msg, "unknown exit_type=%d", et);
      log_error(error_msg);
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
    char error_msg[2048];

    if (msg != NULL) {
      log_error(msg);
    } 
    sprintf(error_msg, "return_code=%ld\nerrno=%d\nmessage=%s\n", 
        return_code, myerrno, error_str);
    log_error(error_msg);

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

size_t read_and_store_string(int client_socket, char **result) {
  uint32_t ulen_net = 0;
  size_t bytes_received = recv(client_socket, &ulen_net, sizeof(ulen_net), 0);
  handle_error(bytes_received, "Recive msg from client", THREAD_EXIT);

  if (bytes_received != sizeof(ulen_net)) {
    handle_error(bytes_received, "Recive msg from client", THREAD_EXIT);
    log_error("recv() failed or connection closed prematurely");
    exit_by_type(THREAD_EXIT);
  }

  uint32_t ulen = ntohl(ulen_net);
  if (ulen == 0) {
    *result = (char *) "";
    return ulen;
  }
  char *buffer = (char *) malloc(ulen + 1);
  buffer[ulen] = '\000';
  char *ptr = buffer;
  size_t rest_len = ulen;
  while (rest_len > 0) {
    /* Receive up to the read_len bytes from the sender */
    bytes_received = read(client_socket, ptr, rest_len);
    if (bytes_received <= 0) {
      log_error("recv() failed or connection closed prematurely");
      exit_by_type(THREAD_EXIT);
    }
    rest_len -= bytes_received;
    ptr += bytes_received;
  }
  *result = buffer;
  return ulen;
}

void write_string(int client_socket, char *str, size_t len) {
  if (len == -1) {
    len = strlen(str);
  }
  uint32_t ulen = (uint32_t) len;
  if (ulen != len) {
    handle_error_myerrno(-1, EFBIG, "string too long", THREAD_EXIT);
  }
  uint32_t ulen_net = htonl(ulen);

  int retcode = write(client_socket, &ulen_net, sizeof(ulen_net));
  handle_error(retcode, "Send message to client", THREAD_EXIT);

  char *ptr = str;
  size_t rest_len = len;

  while (rest_len > 0) {
    size_t partial_len = write(client_socket, ptr, rest_len);
    rest_len -= partial_len;
    ptr += partial_len;
  }
}
