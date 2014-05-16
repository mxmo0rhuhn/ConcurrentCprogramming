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
        log_debug("content buffer + %c", fc);
        fsm->file.content[fsm->buflen++] = fc;
      }
    }
    action term_content {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("content buffer finished");
        fsm->file.content[fsm->buflen++] = '\000';
      }
    }

# Append the current character to the filename buffer
    action append_filename {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer + %c", fc);
        fsm->file.filename[fsm->buflen++] = fc;
      }
    }
    action term_filename {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer finished");
        fsm->file.filename[fsm->buflen++] = '\000';
      }
    }

# Append the current character to the length buffer
    action append_length {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer + %c", fc);
        fsm->file.length[fsm->buflen++] = fc;
      }
    }
    action term_length {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer finished");
        fsm->file.length[fsm->buflen++] = '\000';
      }
    }

# prepare for a new buffer
    action init { 
      log_debug("new buffer prepared");
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
    update = 'UPDATE ' . filename . ' ' length . '\n' content . '\n' @update;
    create = 'CREATE ' . filename . ' ' length . '\n' content . '\n' @create;

main := ( 
          list | 
          read | 
          update |
          delete |
          create
        );
}%%

%% write data;

// Responses
// Errors
#define FILEEXISTS "FILEEXISTS\n"
#define NOSUCHFILE "NOSUCHFILE\n"
#define COMMAND_UNKNOWN "COMMAND_UNKNOWN\n"

// positive responses
#define ACK "ACK"
#define FILECREATED "FILECREATED\n"
#define FILECONTENT "FILECONTENT"
#define DELETED "DELETED\n"
#define UPDATED "UPDATED\n"

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

  size_t len = getAllElemmentIDs(list, &files);

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
  size_t payload_size = atoi(file->length);

  //ignore len if > MAX_BUFLEN
  if(payload_size > MAX_BUFLEN) {
    payload_size = MAX_BUFLEN;
  }

  log_info("Performing CREATE %s, %zu", file->filename, payload_size);
  log_info("Content: %s", file->content);

  payload_size = (payload_size) * sizeof(unsigned char);
  char *content = file->content;

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
  size_t len = getElementByID(list, (void *) &payload, file->filename );

  // Payload check for files with size 0 
  if (len >= 0 || payload != NULL ) {
    // + \000
    char len_c[SIZE_MAX_BUFLEN+1];

    snprintf(len_c, SIZE_MAX_BUFLEN, "%zu", len);

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
  log_info("Performing UPDATE %s", file->filename);

  //ignore len if > MAX_BUFLEN

  return UPDATED;
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
  
  return DELETED;
}

char *handle_message(size_t msg_size, char *msg, ConcurrentLinkedList *file_list) {

  log_debug("handle_message got msg %s", msg);
  log_debug("handle_message got len %d", msg_size);

  struct protocoll protocoll;
  struct protocoll *fsm = &protocoll;
  fsm->buflen = 0;

  %% write init;
  log_debug("init done");

  char *p = msg;
  char *pe = p + msg_size;
  %% write exec;
  log_debug("exec done");

  // save  default
  log_error( "Command unknown: '%s'", msg);
  return COMMAND_UNKNOWN;
}
