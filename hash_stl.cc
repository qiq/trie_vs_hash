#include <tr1/unordered_map>
#include <string.h>
#include <stdlib.h>

#include "algo.h"

using namespace std;

static int keyLen = 0;

class myhash {
public:
        ::std::size_t operator ()(unsigned char * const &x) const {
                size_t __result = static_cast<size_t>(2166136261UL);
                const unsigned char* __cptr = reinterpret_cast<const unsigned char*>(x);
                int __clength = keyLen;
                for (; __clength; --__clength) {
                        __result ^= static_cast<size_t>(*__cptr++);
                        __result *= static_cast<size_t>(16777619UL);
                }
                return __result;
        }
};

class myeq {
public:
        bool operator ()(unsigned char * const &a, unsigned char * const &b) const {
                return memcmp(a, b, keyLen) == 0;
        }
};

typedef struct {
	tr1::unordered_map<unsigned char*, long, myhash, myeq> *map;
	unsigned char *keys;
	int n;
} hash_t;

extern "C" void *algo_new(int len, int items) {
	hash_t *hash = new hash_t();
	keyLen = len;
	hash->map = new tr1::unordered_map<unsigned char*, long, myhash, myeq>();
	hash->map->rehash(items*1.5);
	hash->keys = new unsigned char[items*keyLen];
	hash->n = 0;
	return hash;
}

extern "C" void algo_free(void *algo, int len) {
	hash_t *hash = (hash_t*)algo;
	delete hash->map;
	delete[] hash->keys;
	delete hash;
}

extern "C" void algo_insert(void *algo, int len, unsigned char *key, long value) {
	hash_t *hash = (hash_t*)algo;
	memcpy(hash->keys + hash->n*keyLen, key, keyLen);
	(*hash->map)[hash->keys + hash->n*keyLen] = value;
}

extern "C" long algo_lookup(void *algo, int len, unsigned char *key) {
	hash_t *hash = (hash_t*)algo;
	tr1::unordered_map<unsigned char*, long, myhash, myeq>::iterator iter = hash->map->find(key);
	return iter != hash->map->end() ? iter->second : -1;
}
