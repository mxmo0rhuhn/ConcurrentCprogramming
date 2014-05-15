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

struct protocoll {
  int cs;
  int buflen;
  char length[SIZE_MAX_BUFLEN+1];
  char filename[MAX_BUFLEN+1];
  char content[MAX_BUFLEN+1];
//  char *to_return;
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
        fsm->content[fsm->buflen++] = fc;
      }
    }
    action term_content {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("content buffer finished");
        fsm->content[fsm->buflen++] = '\000';
      }
    }

# Append the current character to the filename buffer
    action append_filename {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer + %c", fc);
        fsm->filename[fsm->buflen++] = fc;
      }
    }
    action term_filename {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer finished");
        fsm->filename[fsm->buflen++] = '\000';
      }
    }

# Append the current character to the length buffer
    action append_length {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer + %c", fc);
        fsm->length[fsm->buflen++] = fc;
      }
    }
    action term_length {
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer finished");
        fsm->length[fsm->buflen++] = '\000';
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
#    action list { fsm->to_return = list_files(); }
#    action read { fsm->to_return = read_file(fsm->filename); }
#    action delete { fsm->to_return = delete_file(fsm->filename); }
#    action update { fsm->to_return = update_file(fsm->filename, fsm->length, fsm->content); }
#    action create { fsm->to_return = create_file(fsm->filename, fsm->length, fsm->content); }
    action list { return list_files(); }
    action read { return read_file(fsm->filename); }
    action delete { return delete_file(fsm->filename); }
    action update { return update_file(fsm->filename, fsm->length, fsm->content); }
    action create { return create_file(fsm->filename, fsm->length, fsm->content); }

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
#define ACK "ACK\n"
#define FILECREATED "FILECREATED\n"
#define FILECONTENT "FILECONTENT\n"
#define DELETED "DELETED\n"
#define UPDATED "UPDATED\n"

/*
 * List all files
 * Possible response:
 *  
 *      ACK NUM_FILES\n
 *      FILENAME\n
 */
char *list_files() {
  log_info("Performing LIST");

  

  return ACK;
}

/*
 * Create a new file
 * Possible response:
 *  
 *      FILEEXISTS\n
 *  or
 *      FILECREATED\n
 */
char *create_file(char *filename, char* length, char *content) {
  log_info("Performing CREATE %s", filename);

  return FILECREATED;
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
char *read_file(char *filename) {
  log_info("Performing READ %s", filename);

  return NOSUCHFILE;
}

/*
 * Change the content of a file
 * Possible response:
 *
 *      NOSUCHFILE\n
 *  or
 *      UPDATED\n
 */
char *update_file(char *filename, char *length, char *content) {
  log_info("Performing UPDATE %s", filename);

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
char *delete_file(char *filename) {
  log_info("Performing DELETE %s", filename);
  
  return DELETED;
}

char *handle_message(size_t msg_size, char *msg, ConcurrentLinkedList *list) {

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
