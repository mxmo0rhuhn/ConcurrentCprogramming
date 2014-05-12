
#line 1 "messageProcessing.rl"

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
#include <concurrentLinkedList.h>

#include <string.h>

// max. size for FILENAME and CONTENT
#define MAX_BUFLEN 1024
// number of chars for the decimal representation of the MAX_BUFLEN
#define SIZE_MAX_BUFLEN 4

struct protocoll {
  int buflen;
  char length[SIZE_MAX_BUFLEN+1];
  char filename[MAX_BUFLEN+1];
  char content[MAX_BUFLEN+1];
}


#line 44 "messageProcessing.c"
static const char _protocoll_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 5, 1, 6, 1, 7, 1, 
	8, 1, 9, 2, 4, 0, 2, 4, 
	1, 2, 4, 2
};

static const char _protocoll_key_offsets[] = {
	0, 0, 5, 6, 7, 8, 9, 10, 
	11, 12, 16, 21, 22, 23, 27, 32, 
	33, 34, 38, 43, 44, 45, 46, 47, 
	48, 49, 50, 51, 52, 56, 61, 62, 
	63, 64, 65, 66, 67, 68, 69, 70, 
	71, 72, 73, 77, 82, 83, 84, 85, 
	86, 87, 88, 89, 90, 91, 95, 100, 
	101, 102, 106, 111, 112, 113, 117, 122, 
	123, 124
};

static const char _protocoll_trans_keys[] = {
	67, 68, 76, 82, 85, 82, 69, 65, 
	84, 69, 32, 0, 97, 117, 108, 110, 
	0, 97, 117, 108, 110, 32, 0, 100, 
	103, 105, 116, 0, 100, 103, 105, 116, 
	10, 0, 97, 117, 108, 110, 0, 97, 
	117, 108, 110, 10, 0, 69, 76, 69, 
	84, 69, 32, 0, 97, 117, 108, 110, 
	0, 97, 117, 108, 110, 10, 0, 73, 
	83, 84, 10, 0, 69, 65, 68, 32, 
	0, 97, 117, 108, 110, 0, 97, 117, 
	108, 110, 10, 0, 80, 68, 65, 84, 
	69, 32, 0, 97, 117, 108, 110, 0, 
	97, 117, 108, 110, 32, 0, 100, 103, 
	105, 116, 0, 100, 103, 105, 116, 10, 
	0, 97, 117, 108, 110, 0, 97, 117, 
	108, 110, 10, 0, 0
};

static const char _protocoll_single_lengths[] = {
	0, 5, 1, 1, 1, 1, 1, 1, 
	1, 2, 3, 1, 1, 4, 5, 1, 
	1, 2, 3, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 2, 3, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 2, 3, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 2, 3, 1, 
	1, 4, 5, 1, 1, 2, 3, 1, 
	1, 0
};

static const char _protocoll_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	0, 0, 0, 0, 0, 1, 1, 0, 
	0, 0
};

static const unsigned char _protocoll_index_offsets[] = {
	0, 0, 6, 8, 10, 12, 14, 16, 
	18, 20, 24, 29, 31, 33, 38, 44, 
	46, 48, 52, 57, 59, 61, 63, 65, 
	67, 69, 71, 73, 75, 79, 84, 86, 
	88, 90, 92, 94, 96, 98, 100, 102, 
	104, 106, 108, 112, 117, 119, 121, 123, 
	125, 127, 129, 131, 133, 135, 139, 144, 
	146, 148, 153, 159, 161, 163, 167, 172, 
	174, 176
};

static const char _protocoll_indicies[] = {
	0, 2, 3, 4, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 1, 10, 1, 
	11, 1, 12, 1, 13, 13, 13, 1, 
	14, 15, 15, 15, 1, 16, 1, 17, 
	1, 18, 18, 18, 18, 1, 19, 20, 
	20, 20, 20, 1, 21, 1, 22, 1, 
	23, 23, 23, 1, 24, 25, 25, 25, 
	1, 26, 1, 27, 1, 28, 1, 29, 
	1, 30, 1, 31, 1, 32, 1, 33, 
	1, 34, 1, 35, 35, 35, 1, 36, 
	37, 37, 37, 1, 38, 1, 39, 1, 
	40, 1, 41, 1, 42, 1, 43, 1, 
	44, 1, 45, 1, 46, 1, 47, 1, 
	48, 1, 49, 1, 50, 50, 50, 1, 
	51, 52, 52, 52, 1, 53, 1, 54, 
	1, 55, 1, 56, 1, 57, 1, 58, 
	1, 59, 1, 60, 1, 61, 1, 62, 
	62, 62, 1, 63, 64, 64, 64, 1, 
	65, 1, 66, 1, 67, 67, 67, 67, 
	1, 68, 69, 69, 69, 69, 1, 70, 
	1, 71, 1, 72, 72, 72, 1, 73, 
	74, 74, 74, 1, 75, 1, 76, 1, 
	1, 0
};

static const char _protocoll_trans_targs[] = {
	2, 0, 21, 32, 37, 46, 3, 4, 
	5, 6, 7, 8, 9, 10, 11, 10, 
	12, 13, 14, 15, 14, 16, 17, 18, 
	19, 18, 20, 65, 22, 23, 24, 25, 
	26, 27, 28, 29, 30, 29, 31, 65, 
	33, 34, 35, 36, 65, 38, 39, 40, 
	41, 42, 43, 44, 43, 45, 65, 47, 
	48, 49, 50, 51, 52, 53, 54, 55, 
	54, 56, 57, 58, 59, 58, 60, 61, 
	62, 63, 62, 64, 65
};

static const char _protocoll_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 22, 7, 3, 
	0, 0, 25, 7, 5, 0, 0, 19, 
	7, 1, 0, 17, 0, 0, 0, 0, 
	0, 0, 0, 22, 7, 3, 0, 13, 
	0, 0, 0, 0, 9, 0, 0, 0, 
	0, 0, 22, 7, 3, 0, 11, 0, 
	0, 0, 0, 0, 0, 0, 22, 7, 
	3, 0, 0, 25, 7, 5, 0, 0, 
	19, 7, 1, 0, 15
};

static const int protocoll_start = 1;
static const int protocoll_first_final = 65;
static const int protocoll_error = 0;

static const int protocoll_en_main = 1;


#line 44 "messageProcessing.rl"


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

}

/*
 * Create a new file
 * Possible response:
 *  
 *      FILEEXISTS\n
 *  or
 *      FILECREATED\n
 */
char *create_file(char *filename, int length, char *content) {
  log_info("Performing CREATE %s", filename);

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
}

char *handle_message(size_t msg_size, char *msg) {

  char *p = msg;
  char *pe = p + strlen(msg);

  
#line 264 "messageProcessing.c"
	{
	 fsm->cs = protocoll_start;
	}

#line 269 "messageProcessing.c"
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
#line 135 "messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        fsm->content[fsm->buflen++] = (*p);
      }
    }
	break;
	case 1:
#line 142 "messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        fsm->filename[fsm->buflen++] = (*p);
      }
    }
	break;
	case 2:
#line 149 "messageProcessing.rl"
	{
      if ( fsm->buflen < MAX_BUFLEN ) {
        fsm->length[fsm->buflen++] = (*p);
      }
    }
	break;
	case 3:
#line 156 "messageProcessing.rl"
	{
      if ( fsm->buflen < BUFLEN ) {
        fsm->buffer[fsm->buflen++] = '\000';
      }
    }
	break;
	case 4:
#line 163 "messageProcessing.rl"
	{ fsm->buflen = 0; }
	break;
	case 5:
#line 171 "messageProcessing.rl"
	{ return list_files(); }
	break;
	case 6:
#line 172 "messageProcessing.rl"
	{ return read_file(fsm->filename); }
	break;
	case 7:
#line 173 "messageProcessing.rl"
	{ return delete_file(fsm->filename); }
	break;
	case 8:
#line 174 "messageProcessing.rl"
	{ return update_file(fsm->filename, fsm->length, fsm->content); }
	break;
	case 9:
#line 175 "messageProcessing.rl"
	{ return create_file(fsm->filename, fsm->length, fsm->content); }
	break;
#line 399 "messageProcessing.c"
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

#line 194 "messageProcessing.rl"

  // save  default
  log_error( "Command unknown: '%s'", msg);
  return COMMAND_UNKNOWN;
}

