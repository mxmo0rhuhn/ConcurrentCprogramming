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
#include <termPaperLib.h>

#include <string.h>

ConcurrentLinkedList *newList() {
  ConcurrentLinkedList *list = malloc(sizeof(ConcurrentLinkedList));
  list->firstElement = NULL;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  list->firstElementMutex = mutex;
  return list;
}

/*
 * Indicate interrest for an element 
 */
void useElement(ConcurrentListElement *element) {
  int retcode = pthread_mutex_lock(&element->usageMutex);
  handle_thread_error(retcode, "lock emement mutex", THREAD_EXIT);
}

/*
 * Return an element 
 */
void returnElement(ConcurrentListElement *element) {
  int retcode = pthread_mutex_unlock(&element->usageMutex);
  handle_thread_error(retcode, "unlock emement mutex", THREAD_EXIT);
}

void useFirstElement(ConcurrentLinkedList *list) {

  // As long as the list exists this lock will never end in nirvana
  int retcode = pthread_mutex_lock(&list->firstElementMutex);
  handle_thread_error(retcode, "lock first elements mutex", THREAD_EXIT);

  ConcurrentListElement *element = list->firstElement;
}

void returnFirstElement(ConcurrentLinkedList *list) {

  // As long as the list exists this lock will never end in nirvana
  int retcode = pthread_mutex_unlock(&list->firstElementMutex);
  handle_thread_error(retcode, "unlock first elements mutex", THREAD_EXIT);
}

ConcurrentListElement *removeElement(ConcurrentListElement *element) {
  useElement(element);
  ConcurrentListElement *next = element->nextEntry;

  // no other thread can access the elemnt right now since the predecessor
  // is locked
  returnElement(element);
  // Pointer and real content
  log_debug("Remove payload: %p", element->payload);
  log_debug("Remove element: %p", element);

  free(element->payload);
  free(element);
  return next;
}

void removeAllElements(ConcurrentLinkedList *list) {
  useFirstElement(list); 
  ConcurrentListElement *first = list->firstElement;

  while(first != NULL ) {
    list->firstElement = removeElement(first);
    first = list->firstElement;
  }
  returnFirstElement(list);
}

void removeFirstListElement(ConcurrentLinkedList *list) {
  useFirstElement(list); 
  ConcurrentListElement *first = list->firstElement;

  if(first != NULL ) {
    list->firstElement = removeElement(first);
    first = list->firstElement;
  }
  returnFirstElement(list);
}

void appendListElement(ConcurrentLinkedList *list, void **payload, 
    size_t payload_size, char* ID) {
  ConcurrentListElement *new = malloc(sizeof(ConcurrentListElement));
  new->payload = malloc(payload_size);
  memcpy(new->payload, *payload, payload_size);

  log_debug("Append payload: %p", *payload);
  log_debug("Append payload size: %d", payload_size);
  log_debug("Append element: %p", new);
  log_debug("Append element payload: %p", new->payload);

  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  new->usageMutex = mutex; 
  new->nextEntry = NULL;
  new->ID=ID; 
  new->payload_size=payload_size;

  useFirstElement(list); 
  ConcurrentListElement *next = list->firstElement;
  ConcurrentListElement *current;
  if(next != NULL ) {

    useElement(next);
    returnFirstElement(list); 
    current = next;
    next = current->nextEntry;
    while(next != NULL ) {
      useElement(next);
      returnElement(current);

      current = next;
      next = current->nextEntry;
    }
    current->nextEntry = new;
    returnElement(current);
  } else {
    list->firstElement = new;   
    returnFirstElement(list); 
  }
}

size_t getFirstListElement(ConcurrentLinkedList *list, void **payload) {
  useFirstElement(list); 
  ConcurrentListElement *first = list->firstElement;
  size_t payload_size;

  if(first != NULL) {

    useElement(first);
    returnFirstElement(list);
    payload_size = first->payload_size;

    *payload = malloc(payload_size);
    memcpy(*payload, first->payload, payload_size);

    log_debug("Original payload: %p", first->payload);
    log_debug("  Return payload: %p", *payload);
    log_debug("Payload size: %d", payload_size);

    returnElement(first);

  } else {
    returnFirstElement(list);
    payload_size = 0; 
    *payload = NULL; 
  }

  return payload_size;
}
