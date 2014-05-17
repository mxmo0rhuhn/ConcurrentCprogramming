/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It is the header for message handeling options
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

#ifndef _MESSAGE_PROCESSING_HEADER
#define _MESSAGE_PROCESSING_HEADER

#include <concurrentLinkedList.h>

/**
 * Handle the given request on the given linked list
 */
char *handle_message(size_t msg_size, char *msg, ConcurrentLinkedList *list) ;

#endif
