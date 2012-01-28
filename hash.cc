#include <stdio.h>
#include <tr1/unordered_map>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <proc/readproc.h>

using namespace std;

static int keyLen = 0;

class myhash {
public:
        ::std::size_t operator ()(char * const &x) const {
                size_t __result = static_cast<size_t>(2166136261UL);
                const char* __cptr = reinterpret_cast<const char*>(x);
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
        bool operator ()(char * const &a, char * const &b) const {
                return memcmp(a, b, keyLen) == 0;
        }
};

//int lens[] = { 4, 6, 8, 10, 12, 14, 16, 0 };
//int ns[] = { 10000, 10000, 20000, 30000, 50000, 100000, 500000, 1000000, 0 };

/* RAND_MAX assumed to be 32767 */
int myrand(void) {
	return (rand() % 32768) << 15 | (rand() % 32768);
}

void usage() {
	fprintf(stderr, "usage: hash keyLen items order random\n");
	exit(1);
}

int main(int argc, char **argv) {
	if (argc != 5)
		usage();
	keyLen = atoi(argv[1]);
	int items = atoi(argv[2]);
	int order = atoi(argv[3]);
	int random = atoi(argv[4]);
	if (!keyLen || !items)
		usage();

	tr1::unordered_map<char*, int, myhash, myeq> map;
	map.rehash(items * 2);
	char *s = new char[items*keyLen];
	memset(s, 0, items*keyLen);
	struct timeval start;
	gettimeofday(&start, NULL);
	if (order) {
		for (int i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s[i*keyLen+0] = key >> 24;
			s[i*keyLen+1] = key >> 16 & 0xFF;
			s[i*keyLen+2] = key >> 8 & 0xFF;
			s[i*keyLen+3] = key & 0xFF;
			map[&s[i*keyLen]] = i;
		}
	} else {
		for (int i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s[i*keyLen+0] = key & 0xFF;
			s[i*keyLen+1] = key >> 8 & 0xFF;
			s[i*keyLen+2] = key >> 16 & 0xFF;
			s[i*keyLen+3] = key >> 24;
			map[&s[i*keyLen]] = i;
		}
	}
	struct timeval t;
	gettimeofday(&t, NULL);
	int64_t insert_time = (int64_t) (t.tv_sec - start.tv_sec) * 1000000 + (t.tv_usec - start.tv_usec);
	char *s2 = new char[keyLen];
	memset(s2, 0, keyLen);
	gettimeofday(&start, NULL);
	int total = 0;
	if (order) {
		for (int i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s2[0] = key >> 24;
			s2[1] = key >> 16 & 0xFF;
			s2[2] = key >> 8 & 0xFF;
			s2[3] = key & 0xFF;
			tr1::unordered_map<char*, int, myhash, myeq>::iterator iter = map.find(s2);
			total += iter->second;
		}
	} else {
		for (int i = 0; i < items; i++) {
			int key = random ? myrand() : i;
			s2[0] = key & 0xFF;
			s2[1] = key >> 8 & 0xFF;
			s2[2] = key >> 16 & 0xFF;
			s2[3] = key >> 24;
			tr1::unordered_map<char*, int, myhash, myeq>::iterator iter = map.find(s2);
			total += iter->second;
		}
	}
	gettimeofday(&t, NULL);
	int64_t lookup_time = (int64_t) (t.tv_sec - start.tv_sec) * 1000000 + (t.tv_usec - start.tv_usec);
	struct proc_t usage;
	look_up_our_self(&usage);
	printf("%ld\t%ld\t%ld\n", insert_time, lookup_time, usage.vsize / (1024*1024));
	delete[] s;
	delete[] s2;

	exit(0);
}
