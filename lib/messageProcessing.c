
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
	1, 10, 2, 1, 11, 2, 3, 8, 
	2, 3, 9, 2, 6, 0, 2, 6, 
	2, 2, 6, 4
};

static const char _protocoll_key_offsets[] = {
	0, 0, 5, 7, 8, 9, 10, 11, 
	12, 18, 25, 27, 30, 36, 43, 44, 
	45, 46, 47, 48, 49, 50, 51, 52, 
	53, 59, 66, 67, 68, 69, 70, 71, 
	72, 73, 74, 80, 87, 88, 89, 90, 
	91, 92, 93, 99, 106, 108, 111, 117, 
	124
};

static const char _protocoll_trans_keys[] = {
	67, 68, 76, 82, 85, 82, 100, 69, 
	65, 84, 69, 32, 48, 57, 65, 90, 
	97, 122, 32, 48, 57, 65, 90, 97, 
	122, 48, 57, 10, 48, 57, 48, 57, 
	65, 90, 97, 122, 10, 48, 57, 65, 
	90, 97, 122, 105, 115, 116, 10, 69, 
	76, 69, 84, 69, 32, 48, 57, 65, 
	90, 97, 122, 10, 48, 57, 65, 90, 
	97, 122, 73, 83, 84, 10, 69, 65, 
	68, 32, 48, 57, 65, 90, 97, 122, 
	10, 48, 57, 65, 90, 97, 122, 80, 
	68, 65, 84, 69, 32, 48, 57, 65, 
	90, 97, 122, 32, 48, 57, 65, 90, 
	97, 122, 48, 57, 10, 48, 57, 48, 
	57, 65, 90, 97, 122, 10, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _protocoll_single_lengths[] = {
	0, 5, 2, 1, 1, 1, 1, 1, 
	0, 1, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 0, 1, 1, 1, 1, 1, 
	1, 1, 0, 1, 0, 1, 0, 1, 
	0
};

static const char _protocoll_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	3, 3, 1, 1, 3, 3, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	3, 3, 0, 0, 0, 0, 0, 0, 
	0, 0, 3, 3, 0, 0, 0, 0, 
	0, 0, 3, 3, 1, 1, 3, 3, 
	0
};

static const unsigned char _protocoll_index_offsets[] = {
	0, 0, 6, 9, 11, 13, 15, 17, 
	19, 23, 28, 30, 33, 37, 42, 44, 
	46, 48, 50, 52, 54, 56, 58, 60, 
	62, 66, 71, 73, 75, 77, 79, 81, 
	83, 85, 87, 91, 96, 98, 100, 102, 
	104, 106, 108, 112, 117, 119, 122, 126, 
	131
};

static const char _protocoll_indicies[] = {
	0, 2, 3, 4, 5, 1, 6, 7, 
	1, 8, 1, 9, 1, 10, 1, 11, 
	1, 12, 1, 13, 13, 13, 1, 14, 
	15, 15, 15, 1, 16, 1, 17, 18, 
	1, 19, 19, 19, 1, 20, 21, 21, 
	21, 1, 22, 1, 23, 1, 24, 1, 
	25, 1, 26, 1, 27, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 32, 
	32, 1, 33, 34, 34, 34, 1, 35, 
	1, 36, 1, 37, 1, 38, 1, 39, 
	1, 40, 1, 41, 1, 42, 1, 43, 
	43, 43, 1, 44, 45, 45, 45, 1, 
	46, 1, 47, 1, 48, 1, 49, 1, 
	50, 1, 51, 1, 52, 52, 52, 1, 
	53, 54, 54, 54, 1, 55, 1, 56, 
	57, 1, 58, 58, 58, 1, 59, 60, 
	60, 60, 1, 1, 0
};

static const char _protocoll_trans_targs[] = {
	2, 0, 18, 26, 30, 36, 3, 14, 
	4, 5, 6, 7, 8, 9, 10, 9, 
	11, 12, 11, 13, 48, 13, 15, 16, 
	17, 48, 19, 20, 21, 22, 23, 24, 
	25, 48, 25, 27, 28, 29, 48, 31, 
	32, 33, 34, 35, 48, 35, 37, 38, 
	39, 40, 41, 42, 43, 44, 43, 45, 
	46, 45, 47, 48, 47
};

static const char _protocoll_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 30, 5, 3, 
	33, 9, 7, 27, 18, 1, 0, 0, 
	0, 13, 0, 0, 0, 0, 0, 0, 
	30, 24, 3, 0, 0, 0, 11, 0, 
	0, 0, 0, 30, 21, 3, 0, 0, 
	0, 0, 0, 0, 30, 5, 3, 33, 
	9, 7, 27, 15, 1
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

  log_info("Performing CREATE %s, %zu", file->filename, (payload_size));
  log_debug("len Filename: %zu", strlen(file->filename));
  log_info("Content: %s", file->content);
  log_debug("len Content: %zu", strlen(file->content));

  // save string with \000
  payload_size++;

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

  log_info("Performing UPDATE %s, %zu", file->filename, (payload_size));
  log_info("Content: %s", file->content);

  // save string with \000
  payload_size++;
  char *content = file->content;

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

  
#line 377 "lib/messageProcessing.c"
	{
	 fsm->cs = protocoll_start;
	}

#line 341 "lib/messageProcessing.rl"

  char *p = msg;
  char *pe = p + msg_size;
  
#line 387 "lib/messageProcessing.c"
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
	_trans = _protocoll_indicies[_trans];
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
#line 547 "lib/messageProcessing.c"
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
