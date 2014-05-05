
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

// Shared Libs aus der Semesterarbeit
#include <termPaperLib.h>
#include <concurrentLinkedList.h>

// stdout 
#include <stdio.h>
//huebsche file fehler
#include <errno.h>
// string stuff
#include <string.h>
// threads
#include <pthread.h>
// exit
#include <stdlib.h>
// read 
#include <unistd.h>
// sockets & co
#include <sys/socket.h>
// addrtype 
#include <netdb.h>
// inet_ntoa
#include <arpa/inet.h>



  if (received_msg_size == CMD_LEN) {
    if (strncmp(*buffer_ptr, CMD, CMD_LEN) != 0) {
      close(payload->socket);    
      handle_thread_error(-1, "CMD not known \n", THREAD_EXIT);
    }
    // wenn es hier vorbei kommt ist alles gut
    log_info("Thread %ld: I know what I shall do", threadID);
  } else {
    close(payload->socket);    
    handle_thread_error(-1, "CMD not known - length not known\n", THREAD_EXIT);
  }
