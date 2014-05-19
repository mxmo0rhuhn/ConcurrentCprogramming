
#line 1 "lib/messageProcessing.rl"
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


#line 145 "lib/messageProcessing.rl"



#line 66 "lib/messageProcessing.c"
static const char _protocoll_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 7, 1, 12, 2, 
	3, 8, 2, 3, 9, 2, 6, 0, 
	2, 6, 2, 2, 6, 4, 3, 0, 
	1, 10, 3, 0, 1, 11
};

static const char _protocoll_key_offsets[] = {
	0, 0, 5, 7, 8, 9, 10, 11, 
	12, 14, 17, 19, 22, 26, 31, 32, 
	33, 34, 35, 36, 37, 38, 39, 40, 
	41, 43, 46, 47, 48, 49, 50, 51, 
	52, 53, 54, 56, 59, 60, 61, 62, 
	63, 64, 65, 67, 70, 72, 75, 79, 
	84, 89, 89
};

static const char _protocoll_trans_keys[] = {
	67, 68, 76, 82, 85, 82, 100, 69, 
	65, 84, 69, 32, 33, 126, 32, 33, 
	126, 48, 57, 10, 48, 57, 9, 13, 
	32, 126, 10, 9, 13, 32, 126, 105, 
	115, 116, 10, 69, 76, 69, 84, 69, 
	32, 33, 126, 10, 33, 126, 73, 83, 
	84, 10, 69, 65, 68, 32, 33, 126, 
	10, 33, 126, 80, 68, 65, 84, 69, 
	32, 33, 126, 32, 33, 126, 48, 57, 
	10, 48, 57, 9, 13, 32, 126, 10, 
	9, 13, 32, 126, 10, 9, 13, 32, 
	126, 10, 9, 13, 32, 126, 0
};

static const char _protocoll_single_lengths[] = {
	0, 5, 2, 1, 1, 1, 1, 1, 
	0, 1, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 0, 1, 1, 1, 1, 1, 
	1, 1, 0, 1, 0, 1, 0, 1, 
	1, 0, 1
};

static const char _protocoll_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 1, 1, 2, 2, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 1, 2, 2, 
	2, 0, 2
};

static const unsigned char _protocoll_index_offsets[] = {
	0, 0, 6, 9, 11, 13, 15, 17, 
	19, 21, 24, 26, 29, 32, 36, 38, 
	40, 42, 44, 46, 48, 50, 52, 54, 
	56, 58, 61, 63, 65, 67, 69, 71, 
	73, 75, 77, 79, 82, 84, 86, 88, 
	90, 92, 94, 96, 99, 101, 104, 107, 
	111, 115, 116
};

static const char _protocoll_trans_targs[] = {
	2, 18, 26, 30, 36, 0, 3, 14, 
	0, 4, 0, 5, 0, 6, 0, 7, 
	0, 8, 0, 9, 0, 10, 9, 0, 
	11, 0, 12, 11, 0, 13, 13, 0, 
	48, 13, 13, 0, 15, 0, 16, 0, 
	17, 0, 49, 0, 19, 0, 20, 0, 
	21, 0, 22, 0, 23, 0, 24, 0, 
	25, 0, 49, 25, 0, 27, 0, 28, 
	0, 29, 0, 49, 0, 31, 0, 32, 
	0, 33, 0, 34, 0, 35, 0, 49, 
	35, 0, 37, 0, 38, 0, 39, 0, 
	40, 0, 41, 0, 42, 0, 43, 0, 
	44, 43, 0, 45, 0, 46, 45, 0, 
	47, 47, 0, 50, 47, 47, 0, 48, 
	13, 13, 0, 0, 50, 47, 47, 0, 
	0
};

static const char _protocoll_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 24, 0, 5, 3, 0, 
	27, 0, 9, 7, 0, 21, 21, 0, 
	34, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 13, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	24, 0, 18, 3, 0, 0, 0, 0, 
	0, 0, 0, 11, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 24, 0, 15, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 24, 0, 
	5, 3, 0, 27, 0, 9, 7, 0, 
	21, 21, 0, 30, 1, 1, 0, 34, 
	1, 1, 0, 0, 30, 1, 1, 0, 
	0
};

static const int protocoll_start = 1;
static const int protocoll_first_final = 48;
static const int protocoll_error = 0;

static const int protocoll_en_main = 1;


#line 148 "lib/messageProcessing.rl"
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

  
#line 369 "lib/messageProcessing.c"
	{
	 fsm->cs = protocoll_start;
	}

#line 341 "lib/messageProcessing.rl"

  char *p = msg;
  char *pe = p + msg_size;
  
#line 379 "lib/messageProcessing.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if (  fsm->cs == 0 )
		goto _out;
_resume:
	_keys = _protocoll_trans_keys + _protocoll_key_offsets[ fsm->cs];
	_trans = _protocoll_index_offsets[ fsm->cs];

	_klen = _protocoll_single_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _protocoll_range_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	 fsm->cs = _protocoll_trans_targs[_trans];

	if ( _protocoll_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _protocoll_actions + _protocoll_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 66 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen < MAX_BUFLEN ) {
      fsm->file.content[fsm->buflen] = (*p);
    }
    fsm->buflen++;
  }
	break;
	case 1:
#line 72 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen <= MAX_BUFLEN ) {
      fsm->file.content[fsm->buflen++] = '\000';
    } else {
      return CONTENT_TO_LONG;
    }
  }
	break;
	case 2:
#line 81 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen < MAX_BUFLEN ) {
      fsm->file.filename[fsm->buflen] = (*p);
    }
    fsm->buflen++;
  }
	break;
	case 3:
#line 88 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen <= MAX_BUFLEN ) {
      fsm->file.filename[fsm->buflen++] = '\000';
    } else {
      return FILENAME_TO_LONG;
    }
  }
	break;
	case 4:
#line 97 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen < SIZE_MAX_BUFLEN ) {
      fsm->file.length[fsm->buflen] = (*p);
    }
    fsm->buflen++;
  }
	break;
	case 5:
#line 104 "lib/messageProcessing.rl"
	{
    if ( fsm->buflen <= SIZE_MAX_BUFLEN ) {
      fsm->file.length[fsm->buflen++] = '\000';
    } 
  // File Len will be validated later
  }
	break;
	case 6:
#line 112 "lib/messageProcessing.rl"
	{ 
    fsm->buflen = 0; 
  }
	break;
	case 7:
#line 122 "lib/messageProcessing.rl"
	{ return list_files(file_list); }
	break;
	case 8:
#line 123 "lib/messageProcessing.rl"
	{ return read_file(file_list, &fsm->file); }
	break;
	case 9:
#line 124 "lib/messageProcessing.rl"
	{ return delete_file(file_list, &fsm->file); }
	break;
	case 10:
#line 125 "lib/messageProcessing.rl"
	{ return update_file(file_list, &fsm->file); }
	break;
	case 11:
#line 126 "lib/messageProcessing.rl"
	{ return create_file(file_list, &fsm->file); }
	break;
	case 12:
#line 133 "lib/messageProcessing.rl"
	{ return "FTW ;-)\n"; }
	break;
#line 538 "lib/messageProcessing.c"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 345 "lib/messageProcessing.rl"

  // save  default
  log_error( "Command unknown: '%s'", msg);
  return COMMAND_UNKNOWN;
}
