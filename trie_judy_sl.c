#include <stdlib.h>
#include <string.h>

#include <Judy.h>

typedef struct {
	Pvoid_t   PJArray;
} trie_t;

void *algo_new(int len, int items)
{
	trie_t *new_trie;
	new_trie = (trie_t*)malloc(sizeof(trie_t));
	if (new_trie == NULL) {
		fprintf(stderr, "malloc failed (out of memmory) in trie_new\n");
		exit(1);
	}
	new_trie->PJArray = (PWord_t)NULL;
	return new_trie;
}

void algo_free(void *trie, int len)
{
	Word_t Bytes;
	JSLFA(Bytes, ((trie_t*)trie)->PJArray);
	free(trie);	/* Free the trie */
}


void algo_insert(void *trie, int len, unsigned char *key, long value)
{
	PWord_t PValue; 
	JSLI(PValue, ((trie_t*)trie)->PJArray, key); 
	*PValue = value;
}


long algo_lookup(void *trie, int len, unsigned char *key)
{
	PWord_t PValue; 
	JSLG(PValue, ((trie_t*)trie)->PJArray, key);
	return PValue ? *PValue : -1;
}
