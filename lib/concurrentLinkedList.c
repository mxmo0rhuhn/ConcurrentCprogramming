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

/**
 * Indicate interrest for the first element of a list
 */
void useFirstElement(ConcurrentLinkedList *list) {

  // As long as the list exists this lock will never end in nirvana
  int retcode = pthread_mutex_lock(&list->firstElementMutex);
  handle_thread_error(retcode, "lock first elements mutex", THREAD_EXIT);

  ConcurrentListElement *element = list->firstElement;
}

/**
 * Return the first element of a list
 */
void returnFirstElement(ConcurrentLinkedList *list) {

  // As long as the list exists this lock will never end in nirvana
  int retcode = pthread_mutex_unlock(&list->firstElementMutex);
  handle_thread_error(retcode, "unlock first elements mutex", THREAD_EXIT);
}

/**
 * remove a element ATTENTION: It has to be ensured, that
 * no other thread can access the elemnt right now (the predecessor 
 * has to be locked)
 */
ConcurrentListElement *removeElement(ConcurrentListElement *element) {
  useElement(element);
  ConcurrentListElement *next = element->nextEntry;

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

  size_t payload_size;

  useFirstElement(list); 
  ConcurrentListElement *first = list->firstElement;

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

size_t getAllElemmentIDs(ConcurrentLinkedList *list, char **IDs) {

  log_debug("getAllElemmentIDs list: %p", list);
  size_t num_elem = 0;
  char *buffer = "\000";

  useFirstElement(list); 
  ConcurrentListElement *next = list->firstElement;
  ConcurrentListElement *current;

  if(next != NULL ) {

    useElement(next);

    num_elem++;
    join_with_seperator(buffer, next->ID, "\n");

    returnFirstElement(list); 
    current = next;
    next = current->nextEntry;

    while(next != NULL ) {
      useElement(next);

      num_elem++;
      join_with_seperator(buffer, next->ID, "\n");

      returnElement(current);
      current = next;
      next = current->nextEntry;
    }
    returnElement(current);
  } else {
    // Empty list
    returnFirstElement(list); 
  }

  *IDs = buffer;
  return num_elem;
}

/**
 * Returns a element for the given ID (if existing)
 * this function keeps an active lock on the predecessor so 
 * the element can savely be modifyed
 * IF the predecessor is NULL the returned element is the first 
 * element of the list
 */
ConcurrentListElement *useElementByID(ConcurrentLinkedList *list, ConcurrentListElement **predecessor, char *ID) {

  *predecessor = NULL;
  ConcurrentListElement *return_element = NULL;

  useFirstElement(list); 
  ConcurrentListElement *next = list->firstElement;
  ConcurrentListElement *current;

  if(next != NULL ) {

    if (strcmp(next->ID, ID) == 0) {
      return_element = next;
      next = NULL;
    } else {
      useElement(next);
      returnFirstElement(list); 

      *predecessor = next;
      current = next;
      next = current->nextEntry;
    }

    while(next != NULL ) {

      if (strcmp(next->ID, ID) == 0) {
        return_element = next;
        next = NULL;
      } else {
        useElement(next);
        returnElement(current);

        *predecessor = next;
        current = next;
        next = current->nextEntry;
      }
    } 
  }
  // Keep the lock on the first element active in case of a 
  // empty list => no else

  return return_element;
}

size_t getElementByID(ConcurrentLinkedList *list, void **payload, char *ID) {
  size_t payload_size = 0; 
  *payload = NULL; 
  ConcurrentListElement *elem;
  ConcurrentListElement *predecessor;

  elem = useElementByID(list, &predecessor, ID) ;

  if (elem != NULL) {
    useElement(elem);
    payload_size = elem->payload_size;
    *payload = malloc(payload_size);
    memcpy(*payload, elem->payload, payload_size);
    returnElement(elem);
  }

  if(predecessor != NULL){
    returnElement(predecessor);
  } else {
    returnFirstElement(list);
  }

  return payload_size;
}
