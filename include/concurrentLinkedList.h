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

#define ELEMENT_RETURNED 0
#define NO_ELEMENT_RETURNED 1
#define USE_SUCCESS 0
#define USE_FAILURE  1

// Linked List aus Threads
typedef struct ConcurrentListElement {
  void *payload;
  pthread_mutex_t *usageMutex;
  struct ConcurrentListElement *nextEntry;
  struct ConcurrentListElement *lastEntry;
  int deleted;
} ConcurrentListElement;

typedef struct ConcurrentLinkedList{
  int numEntries;
  ThreadListEntry **firstElement;
  ThreadListEntry **lastElement;
  size_t numElements;
  pthread_mutex_t *mutex;
} ConcurrentLinkedList;

void appendListElement(ConcurrentLinkedList list, void *payload);
int getFirstElement(ConcurrentLinkedList list, void *payload);
void removeFirstElement(ConcurrentLinkedList list);

#endif
