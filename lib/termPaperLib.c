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
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <arpa/inet.h>  
#include <unistd.h> 
#include <stdarg.h>

#include <termPaperLib.h>

// Found no other way to store these parameters
enum logging_type debug_type;
enum logging_type info_type;
enum logging_type error_type;
FILE *log_file;

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

void log_by_type(const char* to_log, enum logging_type lt) {
  switch (lt) {
    case NONE:
      break;
    case WRITE_TO_FILE:
      fprintf(log_file, "%s\n", to_log);
      fflush(log_file);
      break;
    case WRITE_TO_STDOUT:
      printf("%s\n", to_log);
      break;
    case WRITE_TO_STDERR:
      fprintf(stderr, "%s\n", to_log);
      break;
    default:
      printf("Unknown logging_type: %d", lt);
      exit(2);
      break;
  }
}

void log_debug(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char log_line[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(log_line, msg, argptr);
  va_end(argptr);

  log_by_type(join_with_seperator("DEBUG:", log_line, " "), debug_type);
}
void log_info(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char log_line[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(log_line, msg, argptr);
  va_end(argptr);

  log_by_type(join_with_seperator("INFO:", log_line, " "), info_type);
}

void log_error(const char *msg, ...) {
  // this len is a real vulnerability => keep it long
  char log_line[MAX_MSG_LEN+MAX_BUFLEN];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(log_line, msg, argptr);
  va_end(argptr);

  log_by_type(join_with_seperator("ERROR:", log_line, " "), error_type);
}

int open_logfile(const char *pathname) {
  if (log_file == NULL) {

    pathname = join_with_seperator(pathname, ".log", "");
    log_file = fopen(pathname, "w");
    if (log_file == NULL) {
      printf("something went wrong while opening the log file '%s' - exiting\n", pathname);
      exit_by_type(PROCESS_EXIT);
    }
  }
}

enum logging_type get_log_type(int i, char *filename) {
  switch (i) {
    case 0:
      return NONE;
      break;
    case 1:
      open_logfile(filename);
      return WRITE_TO_FILE;
      break;
    case 2:
      return WRITE_TO_STDOUT;
      break;
    case 3:
      return WRITE_TO_STDERR;
      break;
    default:
      log_error("Unknown log_level: %d", i);
      exit(2);
      break;
  }
}

void get_logging_properties(int argc, char *argv[]) {
  debug_type = NONE;
  info_type = WRITE_TO_STDOUT;
  error_type = WRITE_TO_STDERR;
  log_file = NULL;

  int i;
  for (i = 1; i < argc; i++)  {
    if (strcmp(argv[i], "-d") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        debug_type = get_log_type(atoi(argv[i]), argv[0]);
      } else {
        printf("please provide a logging level for DEBUG for help use -h");
      }
    } else if (strcmp(argv[i], "-i") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        info_type = get_log_type(atoi(argv[i]), argv[0]);
      } else {
        printf("please provide a logging level for INFO for help use -h");
      }
    } else if (strcmp(argv[i], "-e") == 0)  {
      if (i + 2 <= argc )  {
        i++;
        error_type = get_log_type(atoi(argv[i]), argv[0]);
      } else {
        printf("please provide a logging level for ERROR for help use -h");
      }
    } 
  }
}

/* 
 * helper function for dealing with errors 
 */
void handle_error_myerrno(long return_code, int myerrno, const char *msg, 
                            enum exit_type et) {
  if (return_code < 0) {
    const char *error_str = strerror(myerrno);

    if (msg != NULL) {
      log_error("%s", msg);
    } 
    log_error("Return code=%ld", return_code);
    log_error("Errno=%d", error_str);
    log_error("Message=%s", myerrno);

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
  bytes_received = recv(client_socket, buffer, message_max_len,0);
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
  size_t partial_len = send(client_socket, str, len, 0);
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
void strn_add(char** original, char *append) {
  *original = join_with_seperator(*original, append, "\n");
}

char *get_logging_help(char **usage_text) {
  *usage_text=join_with_seperator(*usage_text, "[-d Out] [-i Out] [-e Out]", " ");

  char *help_text = join_with_seperator( 
                       "[-d Loglevel] Optional: Alter the output for DEBUG messages.",
                       "               Default: No logging\n" , "\n" ); 
  strn_add(&help_text, "[-i Loglevel] Optional: Alter the output for INFO messages.");
  strn_add(&help_text, "               Default: stdout\n" );
  strn_add(&help_text, "[-i Loglevel] Optional: Alter the output for ERROR messages.");
  strn_add(&help_text, "               Default: stderr \n" );
  strn_add(&help_text, " Possible log Outputs: 0 = No logging" );
  strn_add(&help_text, "                       1 = Logfile" );
  strn_add(&help_text, "                       2 = stdout" );
  strn_add(&help_text, "                       3 = stderr\n" );

  return help_text;
}

