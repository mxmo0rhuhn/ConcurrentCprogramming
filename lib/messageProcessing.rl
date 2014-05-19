/* 
 * This file is part of the concurrent programming in C term paper
 *
 * It handles messages from the client and starts the requested processing
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

#include <termPaperLib.h>
#include <messageProcessing.h>

#include <string.h>
#include <stdio.h>

// Responses
// Errors
#define FILEEXISTS "FILEEXISTS\n"
#define NOSUCHFILE "NOSUCHFILE\n"

// Not used in the protocol
#define COMMAND_UNKNOWN "COMMAND_UNKNOWN\n"
#define FILENAME_TO_LONG "FILENAME_TO_LONG\n"
#define CONTENT_TO_LONG "CONTENT_TO_LONG\n"

// positive responses
#define ACK "ACK"
#define FILECREATED "FILECREATED\n"
#define FILECONTENT "FILECONTENT"
#define DELETED "DELETED\n"
#define UPDATED "UPDATED\n"


typedef struct file {
  char length[SIZE_MAX_BUFLEN+1];
  char filename[MAX_BUFLEN+1];
  char content[MAX_BUFLEN+1];
} File;

struct protocoll {
  int cs;
  int buflen;
  File file;
};

%%{
# Machine definition
  machine protocoll;

# get the struct
  access fsm->;

# Append the current character to the content buffer
  action append_content {
    if ( fsm->buflen < MAX_BUFLEN ) {
      fsm->file.content[fsm->buflen] = fc;
    }
    fsm->buflen++;
  }
  action term_content {
    if ( fsm->buflen <= MAX_BUFLEN ) {
      fsm->file.content[fsm->buflen++] = '\000';
    } else {
      return CONTENT_TO_LONG;
    }
  }

# Append the current character to the filename buffer
  action append_filename {
    if ( fsm->buflen < MAX_BUFLEN ) {
      fsm->file.filename[fsm->buflen] = fc;
    }
    fsm->buflen++;
  }

  action term_filename {
    if ( fsm->buflen <= MAX_BUFLEN ) {
      fsm->file.filename[fsm->buflen++] = '\000';
    } else {
      return FILENAME_TO_LONG;
    }
  }

# Append the current character to the length buffer
  action append_length {
    if ( fsm->buflen < SIZE_MAX_BUFLEN ) {
      fsm->file.length[fsm->buflen] = fc;
    }
    fsm->buflen++;
  }

  action term_length {
    if ( fsm->buflen <= SIZE_MAX_BUFLEN ) {
      fsm->file.length[fsm->buflen++] = '\000';
    } 
  // File Len will be validated later
  }

# prepare for a new buffer
  action init { 
    fsm->buflen = 0; 
  }

# Helpers that collect strings
  length = digit+ >init $append_length %term_length;
  filename = alnum+ >init $append_filename %term_filename;
  content = alnum+ >init $append_content %term_content;

# action definitions
  action list { return list_files(file_list); }
  action read { return read_file(file_list, &fsm->file); }
  action delete { return delete_file(file_list, &fsm->file); }
  action update { return update_file(file_list, &fsm->file); }
  action create { return create_file(file_list, &fsm->file); }

# Machine definition
  list = 'LIST\n'  @list;
  read = 'READ ' . filename . '\n' @read;
  delete = 'DELETE ' . filename . '\n' @delete;
# small instructor test ... will anyone ever see this?
  special = 'Cdist\n' @{ return "FTW ;-)\n"; };
  update = 'UPDATE ' . filename . ' ' . length . '\n' content . '\n' @update;
  create = 'CREATE ' . filename . ' ' . length . '\n' content . '\n' @create;

main := ( 
          list | 
          read | 
          update |
          special |
          delete |
          create
        );
}%%

%% write data;
/**
 * Since many bad people try to cause SigV ...
 */
size_t validate_size(char *len, char *content) {

  size_t payload_size = atoi(len);
  size_t content_size = strlen(content);

  // due to input parsing this should never happen
  if(content_size < 1) {
    return 0;
  }

  // Files with empty content are not specified as possible by the protocoll
  // (there has always to be a content... if size = 0 sth. is wrong)
  if(payload_size < 1) {
    log_error("content len (%zu) < 1", payload_size);
    return 0;
  }

  // due to input parsing this should never happen
  if(content_size > MAX_BUFLEN) {
    log_error("content len (%zu) > MAX_BUFLEN (%zu)",content_size, MAX_BUFLEN);
    return 0;
  }

  if(content_size < payload_size) {
    payload_size = content_size;
  }

  //ignore len if > MAX_BUFLEN
  if(payload_size > MAX_BUFLEN) {
    payload_size = MAX_BUFLEN;
  }

  if(content_size > payload_size) {
    content[payload_size] = '\000';
  }

  return payload_size;
}

/*
 * List all files
 * Possible response:
 *  
 *      ACK NUM_FILES\n
 *      FILENAME\n
 */
char *list_files(ConcurrentLinkedList *list) {
  log_info("Performing LIST");

  // + \000
  char len_c[SIZE_MAX_BUFLEN+1];
  char *files;

  size_t len = getAllElementIDs(list, &files);

  snprintf(len_c, SIZE_MAX_BUFLEN, "%zu", len);

  char *to_return = join_with_seperator(ACK,len_c," ");
  to_return = join_with_seperator(to_return, files, "");
  to_return = join_with_seperator(to_return,"", "\n");

  return to_return;
}

/*
 * Create a new file
 * Possible response:
 *  
 *      FILEEXISTS\n
 *  or
 *      FILECREATED\n
 */
char *create_file(ConcurrentLinkedList *list, File *file) {
  char *to_return = FILECREATED;

  size_t payload_size = validate_size(file->length, file->content);
  if (payload_size < 1) {
    return COMMAND_UNKNOWN;
  }

  char *content = file->content;

  log_info("Performing CREATE %s %zu", file->filename, (payload_size));
  log_debug("len Filename: %zu", strlen(file->filename));
  log_info("Content: %s", content);
  log_debug("len Content: %zu", strlen(content));

  // save string with \000
  payload_size++;


  if(0 != appendUniqueListElement(list, (void *) &content, payload_size, (file->filename))) {
    to_return = FILEEXISTS;
  } 

  return to_return;
}

/*
 * Read the content of a file
 * Possible response:
 *  
 *      NOSUCHFILE\n
 *  or
 *      FILECONTENT FILENAME LENGTH\n
 *      CONTENT
 */
char *read_file(ConcurrentLinkedList *list, File *file) {
  log_info("Performing READ %s", file->filename);

  char *payload;
  char *to_return = NOSUCHFILE;
  size_t payload_size = getElementByID(list, (void *) &payload, file->filename );

  // Payload check for files with size 0 
  if (payload_size > 0 ) {
    // + \000
    char len_c[SIZE_MAX_BUFLEN+1];

    // return LENGTH without \000
    payload_size--;
    snprintf(len_c, (SIZE_MAX_BUFLEN +1), "%zu", payload_size);

    log_debug("strlen filename = %zu", strlen(file->filename));
    to_return = join_with_seperator(FILECONTENT, file->filename, " ");
    to_return = join_with_seperator(to_return, len_c," ");
    to_return = join_with_seperator(to_return, payload,"\n");
    to_return = join_with_seperator(to_return,"", "\n");
  } 
  return to_return;
}

/*
 * Change the content of a file
 * Possible response:
 *
 *      NOSUCHFILE\n
 *  or
 *      UPDATED\n
 */
char *update_file(ConcurrentLinkedList *list, File *file) {

  char *to_return = UPDATED;

  size_t payload_size = validate_size(file->length, file->content);
  if (payload_size < 1) {
    return COMMAND_UNKNOWN;
  }

  char *content = file->content;
  log_info("Performing UPDATE %s, %zu", file->filename, (payload_size));
  log_info("Content: %s", content);

  // save string with \000
  payload_size++;

  if(0 != updateListElementByID(list, (void *) &content, payload_size, (file->filename))) {
    to_return = NOSUCHFILE;
  } 

  return to_return;
}

/*
 * Delete a file
 * Possible response:
 *
 *      NOSUCHFILE\n
 *  or
 *      DELETED\n
 */
char *delete_file(ConcurrentLinkedList *list, File *file) {
  log_info("Performing DELETE %s", file->filename);

  char *to_return = DELETED;

  if(0 != removeListElementByID(list, (file->filename))) {
    to_return = NOSUCHFILE;
  } 

  return to_return;
}

char *handle_message(size_t msg_size, char *msg, ConcurrentLinkedList *file_list) {

  struct protocoll protocoll;
  struct protocoll *fsm = &protocoll;
  fsm->buflen = 0;

  %% write init;

  char *p = msg;
  char *pe = p + msg_size;
  %% write exec;

  // save  default
  log_error( "Command unknown: '%s'", msg);
  return COMMAND_UNKNOWN;
}
