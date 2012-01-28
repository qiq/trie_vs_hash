/* Copyright (C) 2007-2010, GoodData(R) Corporation. All rights reserved. */

/********************************************** 
 * @file fixedkeytrie.h
 *
 * @brief Fast string lookups
 *
 * A trie is a data structure which provides fast mappings from strings
 * to values.
 *
 * To create a new trie, use @ref trie_new.  To destroy a trie,
 * use @ref trie_free.
 *
 * To insert a value into a trie, use @ref trie_insert. To remove a value
 * from a trie, use @ref trie_remove.  
 *
 * To look up a value from its key, use @ref trie_lookup.
 *
 */

#ifndef ALGORITHM_TRIE_H
#define ALGORITHM_TRIE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Trie trie_t;

/********************************************** 
 * Create a new trie.
 *
 * @return                   Pointer to a new trie structure.
 */

trie_t *trie_new();

/********************************************** 
 * Destroy a trie.
 *
 * @param trie               The trie to destroy.
 */

void trie_free(trie_t **trie, int len);

/********************************************** 
 * Insert a new key-value pair into a trie.
 *
 * @param trie               The trie.
 * @param len                The length of a key.
 * @param key                The key to access the new value.
 * @param value              The value (long, but has to be >= 0).
 */

void trie_insert(trie_t *trie, int len, char *key, long value);

/********************************************** 
 * Look up a value from its key in a trie.
 *
 * @param trie               The trie.
 * @param len                The length of a key.
 * @param key                The key.
 *
 * @return                   The value associated with the key, or NULL if
 *                           not found in the trie.
 */

long trie_lookup(trie_t *trie, int len, char *key);

/********************************************** 
 * Returns array of all keys, where position of specific key
 * is defined by value of a key.
 *
 * Be careful - number of elements is equal to maximum value +1
 * (i.e. this function is preferably intended to return keys
 * in case values are representing continuous set).
 *
 * This function allocate len * (max_value + 1) Bytes of memory,
 * so don't forget to free it.
 *
 * @param trie               The trie.
 * @param len                The length of a key.
 * @param max_value          The maximum value (will define size of returned array).
 *
 * @return                   Ptr to an array of keys. Each key is fixed string of length length.
 */

char *trie_all_keys(trie_t *trie, int len, long max_value);

/********************************************** 
 * Add extension string to all nodes specified by the partial key
 * (of length key_len - ext_len).
 *
 * @param trie               The trie.
 * @param key_len            The full length of the trie key.
 * @param ext_len            The length of extension string.
 * @param ext                Ptr to an extension string.
 * @param value              Ptr to an integer representing counter, will be incremented by all key/extension inserts..
 */

void trie_extend_keys(trie_t *trie, int key_len, int ext_len, char *ext, int *value);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_TRIE_H */


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
