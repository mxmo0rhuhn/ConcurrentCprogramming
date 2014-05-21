/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides the header for a library of common functions and is based on 
 * teaching material by Karl Brodowsky that was used for the system 
 * programming course at Zurich University of Applied Sciences 2014
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

// -------------------------------------------------------------------
// config 

// max. lenght for FILENAME and CONTENT 
// (has to be limited for security reasons)
#define MAX_BUFLEN 1024

// max. lenght for the decimal representation of MAX_BUFLEN 
// (=> max. lenght for the protocolls LENGTH argument)
#define SIZE_MAX_BUFLEN 4

// max. lenght of characters other than FILENAME, CONTENT or LENGTH in 
// the protokoll
// Longest message: FILECONTENT FILENAME LENGTH\nCONTENT\n
#define MAX_OTHER 15

// max. lenght for any revived or logged string 
// ATTENTION... may cause a LOT of problems if it is to short
#define MAX_MSG_LEN (MAX_BUFLEN + MAX_BUFLEN + SIZE_MAX_BUFLEN + MAX_OTHER)

// max. number of waiting socket connections
#define MAX_PENDING_CONNECTIONS 100

// frequency for finished request cleanup in seconds
#define CLEANUP_FREQUENCY 10

// -------------------------------------------------------------------

enum exit_type { PROCESS_EXIT, THREAD_EXIT, NO_EXIT };
enum logging_type { NONE, WRITE_TO_FILE, WRITE_TO_STDOUT, WRITE_TO_STDERR };

/*
 * Logging functions for messages
 * on different levels
 */
void log_debug(const char *msg, ...) ;
void log_info(const char *msg, ...) ;
void log_error(const char *msg, ...) ;

/**
 * Parses the commandline parameters for a port
 **/
unsigned short get_port_with_default(int argc, char *argv[], unsigned short default_port);

/**
 * returns a help text for the port parameter
 **/
char *get_port_help(char **usage_text);

/**
 * Parses the commandline parameters for logging properties
 **/
void get_logging_properties(int argc, char *argv[]);

/**
 * returns a help text for the possible logging options
 **/
char *get_logging_help(char **usage_text);

/* 
 * check if --help or similar is indicated 
 */
int is_help_requested(int argc, char *argv[]);

/* 
 * helper function for thread error handling
 */
void handle_thread_error(int retcode, const char *msg, enum exit_type et);

/* 
 * helper function for error handling
 */
void handle_error(long return_code, const char *msg, enum exit_type et);

/**
 * Creates a socket that is bound to the given port and ist listening vor incomming
 * connections on it
 **/
int create_server_socket(int port_number) ;

/**
 * connects to a server on the given IP and port 
 */
int create_client_socket(int server_port, char *server_ip) ;

/* 
 * Recive a message via TCP/IP over a given socket
 * the caller has to free the buffer for the result
 */
size_t read_from_socket(int client_socket, char **result) ;

/* 
 * Send a message via TCP/IP over a given socket
 */
void write_to_socket(int client_socket, const char *str);

/*
 * Joins two strings with a given seperator and returns the concatinated string
 */
char *join_with_seperator(const char *str1, const char *str2, const char *sep) ;
#endif
