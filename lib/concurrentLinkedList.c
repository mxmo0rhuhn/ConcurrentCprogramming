/* 
 * This file is part of the concurrent programming in C term paper
 * 
 * It provides the implementation of a Linked List with thread save operations
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

#include <concurrentLinkedList.h> 

/*
 * Indicate interrest for an element 
 * May lead to problems when used on deleted elements
 */
int useElement(ConcurrentListElement element) {
  if (element.deleted) {
    // ... it is just luck, that the element is not already cleaned up 
    // exit fast
    return USE_FAILURE;
    pthread_mutex_lock(threadList->mutex);
    handle_thread_error(retcode, "lock mutex", THREAD_EXIT);
  }
}

int returnElement(ConcurrentListElement) {

}

void appendListElement(ConcurrentLinkedList list, void *payload){

}

int getFirstElement(ConcurrentLinkedList list, void *payload) {
  if(list.numElements < 1) 
  }
}

void removeFirstElement(ConcurrentLinkedList list) {

}
