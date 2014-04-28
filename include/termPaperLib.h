/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides the header for a library of common functions and is based on 
 * teaching material by Karl Brodowsky that was used for the system 
 * programming course at Zurich University of Applied Sciences 2014
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

#ifndef _TERM_PAPER_LIB
#define _TERM_PAPER_LIB

#include <time.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0

#define MAX_BLOCK_COUNT 100000
#define MAX_BLOCK_SIZE  100000

struct string_array {
  char **strings;
  int len;
};

enum exit_type { PROCESS_EXIT, THREAD_EXIT, NO_EXIT };

enum file_type { NOT_EXISTENT, DIRECTORY, REGULAR_FILE, OTHER };
/*
 * Logging functions for messages
 */
void log_debug(const char *msg, ...) ;
void log_info(const char *msg, ...) ;
void log_error(const char *msg, ...) ;

/* 
 * check if --help or similar is indicated 
 */
int is_help_requested(int argc, char *argv[]);

struct timespec get_future(time_t sec, long nsec);

void exit_by_type(enum exit_type et);

void handle_thread_error(int retcode, const char *msg, enum exit_type et);

/* helper function for dealing with errors */
void handle_error(long return_code, const char *msg, enum exit_type et);

void handle_error_myerrno(long return_code, int myerrno, const char *msg, enum exit_type et);

void handle_ptr_error(void *ptr, const char *msg, enum exit_type et);

void die_with_error(char *error_message);

int open_retry_mode(char *file, int flags, mode_t mode, enum exit_type et);

int open_retry(char *file, int flags, enum exit_type et);

enum file_type check_file(const char *file_or_dir_name);

/* creates a file if it does not exist */
int create_if_missing(const char *pathname, mode_t mode);

int is_string_char(char c);

/* read the contents of a file and convert it to an array of strings containing the readable characters interpreted as 8-bit-charset */
struct string_array read_to_array(int fd);

#endif
