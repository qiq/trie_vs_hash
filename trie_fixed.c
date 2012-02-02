/* Copyright (C) 2007-2010, GoodData(R) Corporation. All rights reserved. */
/* Trie: fast mapping of strings to values */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "algo.h"

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

typedef struct _Trie trie_t;

/* ----------------------------------------------------------------------- */

void *algo_new(int len, int items)
{
	trie_t *new_trie;
	new_trie = (trie_t*)malloc(sizeof(trie_t));
	if (new_trie == NULL) {
		fprintf(stderr, "malloc failed (out of memmory) in trie_new");
		exit(1);
	}
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

void algo_free(void *trie, int len)
{
	/* Free the subnode, and all others by implication */
	if (len>0) _trie_free_node(&((trie_t*)trie)->root_node, len);
	free(trie);	/* Free the trie */
}

/* ----------------------------------------------------------------------- */

void _trie_insert(TrieNode **rover, int len, unsigned char *key, long value)
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
			if (node == NULL) {
				fprintf(stderr, "calloc failed (out of memmory) in _trie_insert");
				exit(1);
			}
			/* Link in to the trie */
			*rover = node;
		}

		/* Advance to the next node in the chain */
		rover = &node->next[(unsigned char) key[--ki]];
	}
}

void algo_insert(void *trie, int len, unsigned char *key, long value)
{
	_trie_insert(&((trie_t*)trie)->root_node, len, key, value);
}

/* ----------------------------------------------------------------------- */

long algo_lookup(void *trie, int len, unsigned char *key)
{
	TrieNode *node;
	node = ((trie_t*)trie)->root_node;

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
