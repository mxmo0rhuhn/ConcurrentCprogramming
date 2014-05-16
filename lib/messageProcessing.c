
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


#line 122 "lib/messageProcessing.rl"



#line 48 "lib/messageProcessing.c"
static const char _protocoll_actions[] = {
	0, 1, 0, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 7, 2, 1, 10, 
	2, 1, 11, 2, 3, 8, 2, 3, 
	9, 2, 6, 0, 2, 6, 2, 2, 
	6, 4
};

static const char _protocoll_key_offsets[] = {
	0, 0, 5, 6, 7, 8, 9, 10, 
	11, 17, 24, 26, 29, 35, 42, 43, 
	44, 45, 46, 47, 48, 54, 61, 62, 
	63, 64, 65, 66, 67, 68, 69, 75, 
	82, 83, 84, 85, 86, 87, 88, 94, 
	101, 103, 106, 112, 119
};

static const char _protocoll_trans_keys[] = {
	67, 68, 76, 82, 85, 82, 69, 65, 
	84, 69, 32, 48, 57, 65, 90, 97, 
	122, 32, 48, 57, 65, 90, 97, 122, 
	48, 57, 10, 48, 57, 48, 57, 65, 
	90, 97, 122, 10, 48, 57, 65, 90, 
	97, 122, 69, 76, 69, 84, 69, 32, 
	48, 57, 65, 90, 97, 122, 10, 48, 
	57, 65, 90, 97, 122, 73, 83, 84, 
	10, 69, 65, 68, 32, 48, 57, 65, 
	90, 97, 122, 10, 48, 57, 65, 90, 
	97, 122, 80, 68, 65, 84, 69, 32, 
	48, 57, 65, 90, 97, 122, 32, 48, 
	57, 65, 90, 97, 122, 48, 57, 10, 
	48, 57, 48, 57, 65, 90, 97, 122, 
	10, 48, 57, 65, 90, 97, 122, 0
};

static const char _protocoll_single_lengths[] = {
	0, 5, 1, 1, 1, 1, 1, 1, 
	0, 1, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 0, 1, 
	1, 1, 1, 1, 1, 1, 0, 1, 
	0, 1, 0, 1, 0
};

static const char _protocoll_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	3, 3, 1, 1, 3, 3, 0, 0, 
	0, 0, 0, 0, 3, 3, 0, 0, 
	0, 0, 0, 0, 0, 0, 3, 3, 
	0, 0, 0, 0, 0, 0, 3, 3, 
	1, 1, 3, 3, 0
};

static const unsigned char _protocoll_index_offsets[] = {
	0, 0, 6, 8, 10, 12, 14, 16, 
	18, 22, 27, 29, 32, 36, 41, 43, 
	45, 47, 49, 51, 53, 57, 62, 64, 
	66, 68, 70, 72, 74, 76, 78, 82, 
	87, 89, 91, 93, 95, 97, 99, 103, 
	108, 110, 113, 117, 122
};

static const char _protocoll_indicies[] = {
	0, 2, 3, 4, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 12, 12, 1, 13, 14, 
	14, 14, 1, 15, 1, 16, 17, 1, 
	18, 18, 18, 1, 19, 20, 20, 20, 
	1, 21, 1, 22, 1, 23, 1, 24, 
	1, 25, 1, 26, 1, 27, 27, 27, 
	1, 28, 29, 29, 29, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 38, 
	38, 1, 39, 40, 40, 40, 1, 41, 
	1, 42, 1, 43, 1, 44, 1, 45, 
	1, 46, 1, 47, 47, 47, 1, 48, 
	49, 49, 49, 1, 50, 1, 51, 52, 
	1, 53, 53, 53, 1, 54, 55, 55, 
	55, 1, 1, 0
};

static const char _protocoll_trans_targs[] = {
	2, 0, 14, 22, 26, 32, 3, 4, 
	5, 6, 7, 8, 9, 10, 9, 11, 
	12, 11, 13, 44, 13, 15, 16, 17, 
	18, 19, 20, 21, 44, 21, 23, 24, 
	25, 44, 27, 28, 29, 30, 31, 44, 
	31, 33, 34, 35, 36, 37, 38, 39, 
	40, 39, 41, 42, 41, 43, 44, 43
};

static const char _protocoll_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 28, 5, 3, 31, 
	9, 7, 25, 16, 1, 0, 0, 0, 
	0, 0, 0, 28, 22, 3, 0, 0, 
	0, 11, 0, 0, 0, 0, 28, 19, 
	3, 0, 0, 0, 0, 0, 0, 28, 
	5, 3, 31, 9, 7, 25, 13, 1
};

static const int protocoll_start = 1;
static const int protocoll_first_final = 44;
static const int protocoll_error = 0;

static const int protocoll_en_main = 1;


#line 125 "lib/messageProcessing.rl"

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

  
#line 298 "lib/messageProcessing.c"
	{
	 fsm->cs = protocoll_start;
	}

#line 265 "lib/messageProcessing.rl"
  log_debug("init done");

  char *p = msg;
  char *pe = p + msg_size;
  
#line 309 "lib/messageProcessing.c"
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
#line 48 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("content buffer + %c", (*p));
        fsm->file.content[fsm->buflen++] = (*p);
      }
    }
	break;
	case 1:
#line 54 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("content buffer finished");
        fsm->file.content[fsm->buflen++] = '\000';
      }
    }
	break;
	case 2:
#line 62 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer + %c", (*p));
        fsm->file.filename[fsm->buflen++] = (*p);
      }
    }
	break;
	case 3:
#line 68 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("filename buffer finished");
        fsm->file.filename[fsm->buflen++] = '\000';
      }
    }
	break;
	case 4:
#line 76 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer + %c", (*p));
        fsm->file.length[fsm->buflen++] = (*p);
      }
    }
	break;
	case 5:
#line 82 "lib/messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        log_debug("len buffer finished");
        fsm->file.length[fsm->buflen++] = '\000';
      }
    }
	break;
	case 6:
#line 90 "lib/messageProcessing.rl"
	{ 
      log_debug("new buffer prepared");
      fsm->buflen = 0; 
    }
	break;
	case 7:
#line 102 "lib/messageProcessing.rl"
	{ return list_files(file_list); }
	break;
	case 8:
#line 103 "lib/messageProcessing.rl"
	{ return read_file(file_list, &fsm->file); }
	break;
	case 9:
#line 104 "lib/messageProcessing.rl"
	{ return delete_file(file_list, &fsm->file); }
	break;
	case 10:
#line 105 "lib/messageProcessing.rl"
	{ return update_file(file_list, &fsm->file); }
	break;
	case 11:
#line 106 "lib/messageProcessing.rl"
	{ return create_file(file_list, &fsm->file); }
	break;
#line 464 "lib/messageProcessing.c"
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

#line 270 "lib/messageProcessing.rl"
  log_debug("exec done");

  // save  default
  log_error( "Command unknown: '%s'", msg);
  return COMMAND_UNKNOWN;
}
