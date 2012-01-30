/* Copyright (C) 2007-2010, GoodData(R) Corporation. All rights reserved. */
/* Trie: fast mapping of strings to values */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <proc/readproc.h>

#include <Judy.h>

/* ----------------------------------------------------------------------- */

#if defined(__SIZEOF_LONG__) && defined(__SIZEOF_POINTER__) && (__SIZEOF_LONG__ != __SIZEOF_POINTER__)
	#error "size of long is not equal to size of pointer on this platform"
#endif

#if defined(__CHAR_BIT__) && (__CHAR_BIT__ != 8)
	#error "size of char is not 8bit on this platform"
#endif

#define CHAR_CARD 256

typedef struct _trie_t trie_t;

struct _trie_t {
	Pvoid_t   PJArray;
};

int myrand(void) {
	return (rand() % 32768) << 15 | (rand() % 32768);
}

/* ----------------------------------------------------------------------- */

trie_t *trie_new()
{
	trie_t *new_trie;
	new_trie = (trie_t *) malloc(sizeof(trie_t));
	if (new_trie == NULL) {
		fprintf(stderr, "malloc failed (out of memmory) in trie_new");
		exit(1);
	}
	new_trie->PJArray = (PWord_t)NULL;
	return new_trie;
}

/* ----------------------------------------------------------------------- */

void trie_free(trie_t **trie, int len)
{
	trie_t *t = *trie;

	Word_t Bytes;
	JSLFA(Bytes, t->PJArray);
	free(*trie);	/* Free the trie */
	*trie = NULL;
}

/* ----------------------------------------------------------------------- */

void trie_insert(trie_t *trie, int len, char *key, long value)
{

	PWord_t PValue; 
	JSLI(PValue, trie->PJArray, (unsigned char*)key); 
	*PValue = value;
}

/* ----------------------------------------------------------------------- */

long trie_lookup(trie_t *trie, int len, char *key)
{
	PWord_t PValue; 
	JSLG(PValue, trie->PJArray, (unsigned char*)key);
	return PValue ? *PValue : -1;
}

/* ----------------------------------------------------------------------- */

char *trie_all_keys(trie_t *trie, int len, long max_value)
{
	return NULL;
}


/* ----------------------------------------------------------------------- */

void trie_extend_keys(trie_t *trie, int key_len, int ext_len, char *ext, int *value)
{
	return;
}

void usage() {
	fprintf(stderr, "usage: trie_judy keyLen items order random\n");
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
	//getc(stdin);
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
