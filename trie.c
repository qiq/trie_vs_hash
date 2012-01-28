/* Copyright (C) 2007-2010, GoodData(R) Corporation. All rights reserved. */
/* Trie: fast mapping of strings to values */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <proc/readproc.h>

#include "trie.h"

long memory = 0;

/* ----------------------------------------------------------------------- */

#if defined(__SIZEOF_LONG__) && defined(__SIZEOF_POINTER__) && (__SIZEOF_LONG__ != __SIZEOF_POINTER__)
	#error "size of long is not equal to size of pointer on this platform"
#endif

#if defined(__CHAR_BIT__) && (__CHAR_BIT__ != 8)
	#error "size of char is not 8bit on this platform"
#endif

#define CHAR_CARD 256

typedef struct _TrieNode TrieNode;

struct _TrieNode {
	TrieNode *next[CHAR_CARD];
};

struct _Trie {
	TrieNode *root_node;
};

void croak(char *s) {
	fprintf(stderr, "%s\n", s);
	exit(1);
}

int myrand(void) {
	return (rand() % 32768) << 15 | (rand() % 32768);
}

/* ----------------------------------------------------------------------- */

trie_t *trie_new()
{
	trie_t *new_trie;
	new_trie = (trie_t *) malloc(sizeof(trie_t));
	if (new_trie == NULL) croak("malloc failed (out of memmory) in trie_new");
	new_trie->root_node = NULL;
	return new_trie;
}

/* ----------------------------------------------------------------------- */

static void _trie_free_node(TrieNode **node, int len)
{
	int i;
	TrieNode *n = *node;

	if (*node == NULL) return;

	if (--len >= 1) {
		/* Free all subnodes */
		for (i=0; i<CHAR_CARD; ++i) {
			_trie_free_node(&n->next[i], len);
		}
	}
	free(*node);	/* Free this node */
	*node = NULL;
}

void trie_free(trie_t **trie, int len)
{
	trie_t *t = *trie;

	if (*trie == NULL) return;
	/* Free the subnode, and all others by implication */
	if (len>0) _trie_free_node(&t->root_node, len);
	free(*trie);	/* Free the trie */
	*trie = NULL;
}

/* ----------------------------------------------------------------------- */

void _trie_insert(TrieNode **rover, int len, char *key, long value)
{
	/* Traversing down the trie, creating nodes as necessary */
	int ki = len;
	for (;;) {

		TrieNode *node;
		node = *rover;

		/* Reached the end of array?  If so, we're finished. */
		if (ki == 0) {
			/* Set the data at the node we have reached */
			*rover = (TrieNode *) ++value;
			break;
		}

		if (node == NULL) {
			/* Node does not exist, so create it */
			node = (TrieNode *) calloc(1, sizeof(TrieNode));
			if (node == NULL) croak("calloc failed (out of memmory) in _trie_insert");
			memory += sizeof(TrieNode);

			/* Link in to the trie */
			*rover = node;
		}

		/* Advance to the next node in the chain */
		rover = &node->next[(unsigned char) key[--ki]];
	}
}

void trie_insert(trie_t *trie, int len, char *key, long value)
{
	_trie_insert(&trie->root_node, len, key, value);
}

/* ----------------------------------------------------------------------- */

long trie_lookup(trie_t *trie, int len, char *key)
{
	TrieNode *node;
	node = trie->root_node;

	/* Search down the trie */
	int ki = len;
	while (ki > 0) {
		if (node == NULL) /* Not found - reached end of branch */
			return -1;

		/* Advance to the next node in the chain */
		node = node->next[(unsigned char) key[--ki]];
	}

	return (long) node - 1;
}

/* ----------------------------------------------------------------------- */

static void _trie_all_keys(TrieNode *node, int key_len, char *ret, int level, char *kb)
{
	int i;

	if (node == NULL) return;

	if (level > 1) {
		for (i=0; i<CHAR_CARD; ++i) {
			*(kb+level-1) = (char) i;
			_trie_all_keys(node->next[i], key_len, ret, level-1, kb);
		}
	} else {	/* we are on the lower level of the trie */
		for (i=0; i<CHAR_CARD; ++i) {
			long val = (long) node->next[i];
			if (--val >= 0) {	/* this key has value set */
				*kb = (char) i;
				memcpy(ret+(val*key_len), kb, key_len);
			}
		}
	}
}

char *trie_all_keys(trie_t *trie, int len, long max_value)
{
	char *ret;
	ret = (char *) calloc(1, len * (max_value + 1));
	if (ret == NULL) croak("calloc failed (out of memmory) in trie_all_keys");

	char kb[len];	/* key buffer */
	_trie_all_keys(trie->root_node, len, ret, len, (char *) &kb);

	return ret;
}


/* ----------------------------------------------------------------------- */

static void _trie_extend_keys(TrieNode *node, int key_len, int level, int ext_len, char *ext, int *value)
{
	int i;

	if (node == NULL) return;

	if (level > ext_len+1) {
		for (i=0; i<CHAR_CARD; ++i) {
			_trie_extend_keys(node->next[i], key_len, level-1, ext_len, ext, value);
		}
	} else {	/* we are on the lower partial level of the trie */
		for (i=0; i<CHAR_CARD; ++i) {
			if (node->next[i] != NULL) {
				_trie_insert(&node->next[i], ext_len, ext, ++*value);
			}
		}
	}
}

void trie_extend_keys(trie_t *trie, int key_len, int ext_len, char *ext, int *value)
{
	_trie_extend_keys(trie->root_node, key_len, key_len, ext_len, ext, value);
}

void usage() {
	fprintf(stderr, "usage: trie keyLen items order random\n");
	exit(1);
}

static int keyLen = 0;

int main(int argc, char **argv) {
	if (argc != 5)
		usage();
	keyLen = atoi(argv[1]);
	int items = atoi(argv[2]);
	int order = atoi(argv[3]);
	int random = atoi(argv[4]);
	if (!keyLen || !items)
		usage();

	trie_t *tr = trie_new();
	struct timeval start;
	gettimeofday(&start, NULL);

	char *s = (char*)malloc(keyLen);
	memset(s, 0, keyLen);
	int i;
	if (order) {
		for (i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s[0] = key >> 24;
			s[1] = key >> 16 & 0xFF;
			s[2] = key >> 8 & 0xFF;
			s[3] = key & 0xFF;
			trie_insert(tr, keyLen, s, i);
		}
	} else {
		for (i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s[0] = key & 0xFF;
			s[1] = key >> 8 & 0xFF;
			s[2] = key >> 16 & 0xFF;
			s[3] = key >> 24;
			trie_insert(tr, keyLen, s, i);
		}
	}
	struct timeval t;
	gettimeofday(&t, NULL);
	int64_t insert_time = (int64_t) (t.tv_sec - start.tv_sec) * 1000000 + (t.tv_usec - start.tv_usec);

	char *s2 = (char*)malloc(keyLen);
	memset(s, 0, keyLen);
	gettimeofday(&start, NULL);
	int total = 0;
	if (order) {
		for (i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s2[0] = key >> 24;
			s2[1] = key >> 16 & 0xFF;
			s2[2] = key >> 8 & 0xFF;
			s2[3] = key & 0xFF;
		}
	} else {
		for (i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s2[0] = key & 0xFF;
			s2[1] = key >> 8 & 0xFF;
			s2[2] = key >> 16 & 0xFF;
			s2[3] = key >> 24;
			total += trie_lookup(tr, keyLen, s);
		}
	}
	gettimeofday(&t, NULL);
	int64_t lookup_time = (int64_t) (t.tv_sec - start.tv_sec) * 1000000 + (t.tv_usec - start.tv_usec);
	struct proc_t usage;
	look_up_our_self(&usage);
	printf("%ld\t%ld\t%ld\n", insert_time, lookup_time, usage.vsize / (1024*1024));
	free(s);
	free(s2);

	exit(0);
}

/*************************************************************************

This code an adaptation of code from:
http://www.koders.com/c/fidD8A1251145ED9CF1EC1CFBAC158B7035838E5DA8.aspx

Original Copyright notice:

Copyright (c) 2005, Simon Howard
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

 * Redistributions of source code must retain the above copyright 
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
   notice, this list of conditions and the following disclaimer in 
   the documentation and/or other materials provided with the 
   distribution.
 * Neither the name of the C Algorithms project nor the names of its 
   contributors may be used to endorse or promote products derived 
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

*************************************************************************/
