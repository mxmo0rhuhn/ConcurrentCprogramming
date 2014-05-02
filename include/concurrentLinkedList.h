/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It provides the header of a Linked List with thread save operations
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

#ifndef _CONCURRENT_LINKED_LIST
#define _CONCURRENT_LINKED_LIST

#include <pthread.h>
#include <stdlib.h>

// Linked List of threads
typedef struct ConcurrentListElement {
  size_t payload_size;
  void *payload;
  pthread_mutex_t usageMutex;
  char *ID;
  struct ConcurrentListElement *nextEntry;
} ConcurrentListElement;

typedef struct ConcurrentLinkedList {
  pthread_mutex_t firstElementMutex;
  ConcurrentListElement *firstElement;
} ConcurrentLinkedList;

/**
 * Returns a new List
 */
ConcurrentLinkedList *newList() ;

/*
 * Removes all elements that are currently in the list
 */
void removeAllElements(ConcurrentLinkedList *list);

/*
 * Adds an element to the end of the list
 */
void appendListElement(ConcurrentLinkedList *list, void **payload, 
                       size_t payload_size, char* ID) ;

/*
 * Returns a shallow copy of the first element!
 * ATTENTION: This will not secure the integrity of the target of pointers
 *            in the payload of the element!
 * The copy has to be freed by the calling thread
 */
size_t getFirstListElement(ConcurrentLinkedList *list, void **payload);

/**
 * Removes the first element of the List - if existing
 */
void removeFirstListElement(ConcurrentLinkedList *list);
#endif
