/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides library functions for common tasks and is based on teaching 
 * material by Karl Brodowsky that was used for the system programming course 
 * at Zurich University of Applied Sciences 2014
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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

#include <termPaperLib.h>

/*
 * Logging function for an info message
 */
void log_info(const char *msg, ...) {
  char logLine[1024];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("INFO: %s\n", logLine);
}

/*
 * Logging function for an info message
 */
void log_debug(const char *msg, ...) {
  char logLine[1024];
  va_list argptr;
  va_start(argptr, msg);
  vsprintf(logLine, msg, argptr);
  va_end(argptr);
  printf("DEBUG: %s\n", logLine);
}

/*
 * Logging function for an error message 
 */
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


void handle_ptr_error(void *ptr, const char *msg, enum exit_type et) {
  if (ptr == NULL) {
    handle_error(-1L, msg, et);
  }
}

void die_with_error(char *error_message) {
  perror(error_message);
  exit(1);
}

int open_retry_mode(char *file, int flags, mode_t mode, enum exit_type et) {

  int fd = -1;
  while (fd == -1) {
    fd = open(file, flags, mode);
    if (fd < 0 && errno == EMFILE) {
      sleep(1);
      continue;
    }
    if (fd < 0) {
      char msg[1024];
      sprintf(msg, "error while opening file=%s", file);
      handle_error(fd, msg, et);
    }
  }
  return fd;
}

int open_retry(char *file, int flags, enum exit_type et) {

  int fd = -1;
  while (fd == -1) {
    fd = open(file, flags);
    if (fd < 0 && errno == EMFILE) {
      sleep(1);
      continue;
    }
    if (fd < 0) {
      char msg[1024];
      sprintf(msg, "error while opening file=%s", file);
      handle_error(fd, msg, et);
    }
  }
  return fd;
}


/* 
 * check if file exits and what type it is
 * exit with error if errors occur during stat
 * return NOT_EXISTENT, DIRECTORY, REGULAR_FILE or OTHER
 */
enum file_type check_file(const char *file_or_dir_name) {
  int r;
  struct stat stat_buf;
  r = lstat(file_or_dir_name, &stat_buf);
  char msg[1024];

  if (r < 0) {
    int myerrno = errno;
    if (myerrno == ENOENT) {
      /* 
       *not existing should be handled as legitimate result, not as error 
       */
      return NOT_EXISTENT;
    } else {
      sprintf(msg, "error while opening file=%s", file_or_dir_name);
      handle_error(-1, msg, PROCESS_EXIT);
    }
  }
  mode_t st_mode = stat_buf.st_mode;
  if (S_ISDIR(st_mode)) {
    return DIRECTORY;
  } else if (S_ISREG(st_mode)) {
    return REGULAR_FILE;
  } else {
    return OTHER;
  }
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

/* 
 * get a timestamp that is s seconds and n 
 * nanoseconds into the future from now 
 */
struct timespec get_future(time_t sec, long nsec) {
  int retcode;

  struct timespec ts;
  retcode = clock_gettime(CLOCK_REALTIME, &ts);
  handle_error(retcode, "clock_gettime", PROCESS_EXIT);
  ts.tv_sec  += sec;
  ts.tv_nsec += nsec;
  return ts;
}

int is_string_char(char c) {
  return ('A' <= c && c <= 'Z' 
      || '0' <= c && c <= '9' 
      || 'a' <= c && c <= 'z' 
      || '\240' <= c && c <= '\377');
}

/* 
 * read the contents of a file and convert it to an array of strings containing 
 * the readable characters interpreted as 8-bit-charset 
 */
struct string_array read_to_array(int fd) {
  struct stat stat;
  long retcode = fstat(fd, &stat);
  handle_error(retcode, "stat", PROCESS_EXIT);
  if (! S_ISREG(stat.st_mode)) {
    handle_error(-1, "no regular file", PROCESS_EXIT);
  }
  off_t size = stat.st_size;
  // printf("read_to_array size=%ld\n", (long)size);
  char *content = malloc(size + 1);
  content[size] = '\000';
  retcode = read(fd, content, size);
  // printf("read %ld bytes\n", retcode);
  handle_error(retcode, "read", PROCESS_EXIT);
  if (retcode < size) {
    char error[1024];
    sprintf(error, "read %ld characters instead of expected %ld", (long) retcode, (long) size);
    /* TODO we should be more generous with read not taking 
     * all of the file contents at once 
     */
    handle_error(-1, error, PROCESS_EXIT);
  }
  /* temporarily make a two-level structure for the string pointers 
  */
  struct string_array blocks[MAX_BLOCK_COUNT]; // 1048576]; // 1024*1024
  // printf("blocks created\n");
  for (int i = 0; i < MAX_BLOCK_COUNT; i++) {
    blocks[i].len = 0;
    blocks[i].strings = (char **) NULL;
  }
  //printf("blocks initialized\n");
  long string_count = 0;
  long block_count = 0;
  long string_pos = 0;
  long block_pos = 0;

  char *source_pointer = content;
  char *target_pointer = content;
  char *end_pointer = content + size;
  char *string_pointer = content;
  /* 
   * go through everything 
   */
  while (source_pointer <= end_pointer) {
    /* go through one string, ends latest at end_pointer */
    string_pointer = target_pointer;
    while (source_pointer <= end_pointer) {
      char c = *source_pointer;
      source_pointer++;
      if (is_string_char(c)) {
        *target_pointer = c;
        target_pointer++;
      } else {
        *target_pointer = '\000';
        target_pointer++;
        break;
      }
    }
    if (target_pointer > string_pointer) {
      // printf("found string %s (sc=%ld bc=%ld sp=%ld bp=%ld)\n", string_pointer, string_count, block_count, string_pos, block_pos);
      /* real string found */
      struct string_array *block = &(blocks[block_pos]);
      if (block->strings == NULL) {
        // printf("initialzing strings in block_pos=%ld\n", block_pos);
        char **ptr =  (char **) malloc(MAX_BLOCK_SIZE * sizeof(char *));
        handle_ptr_error(ptr, "malloc", PROCESS_EXIT);
        block->strings = ptr;
        block_count++;
      }
      block->strings[string_pos] = string_pointer;
      block->len++;
      string_pos++;
      string_count++;
      if (string_pos >= MAX_BLOCK_SIZE) {
        //printf("incrementing block_pos sp=%ld bp=%ld\n", string_pos, block_pos);
        string_pos = 0;
        block_pos++;
        //printf("incremented block_pos sp=%ld bp=%ld\n", string_pos, block_pos);
      }
    }
    while (source_pointer <= end_pointer) {
      char c = *source_pointer;
      if (is_string_char(c)) {
        break;
      }
      source_pointer++;
    }
  }
  struct string_array result;
  result.len = string_count;
  result.strings = malloc(string_count * sizeof(char *));
  string_pos = 0;
  block_pos = 0;
  for (int i = 0; i < string_count; i++) {
    result.strings[i] = blocks[block_pos].strings[string_pos];
    string_pos++;
    if (string_pos >= MAX_BLOCK_SIZE) {
      string_pos = 0;
      block_pos++;
    }
  }
  for (int i = 0; i < block_count; i++) {
    free(blocks[i].strings);
  }
  return result;
}
