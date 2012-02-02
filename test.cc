#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <proc/readproc.h>
#include <stdint.h>

#include "algo.h"

unsigned char *random_key(unsigned char *s, int len) {
	int i = 0;
	static uint32_t r;
	r = rand() % 32768;
	static int bits = 15;
	for (i = 0; i < len; i++) {
		// we do not want NUL characters
		s[i] = (r & 0xFF) ? r & 0xFF : rand() & 0xFF;
		bits -= 8;
		r >>= 8;
		if (bits < 8) {
			r = (r & ((1<<bits)-1)) | ((rand() % 32768) << bits);
			bits += 15;
		}
	}

	return s;
}

unsigned char *sequence_key1(unsigned char *s, int len, int i) {
	memset(s+4, 1, len-4);
	unsigned char a = i >> 24;
	unsigned char b = (i >> 16) & 0xFF;
	unsigned char c = (i >> 8) & 0xFF;
	unsigned char d = i & 0xFF;

	s[3] = a ? a : 1;
	s[2] = b ? b : 1;
	s[1] = c ? c : 1;
	s[0] = d ? d : 1;

	return s;
}

unsigned char *sequence_key2(unsigned char *s, int len, int i) {
	memset(s+4, 1, len-4);
	unsigned char a = i >> 24;
	unsigned char b = (i >> 16) & 0xFF;
	unsigned char c = (i >> 8) & 0xFF;
	unsigned char d = i & 0xFF;

	s[0] = a ? a : 1;
	s[1] = b ? b : 1;
	s[2] = c ? c : 1;
	s[3] = d ? d : 1;

	return s;
}

void usage() {
	fprintf(stderr, "usage: test keyLen items <sequence1|sequence2|random>\n");
	exit(1);
}

typedef enum {
	SEQUENCE1 = 1,
	SEQUENCE2 = 2,
	RANDOM = 3,
} type_t;

int main(int argc, char **argv) {
	if (argc != 4)
		usage();

	int keyLen = atoi(argv[1]);
	int items = atoi(argv[2]);
	char *type = argv[3];
	if (!keyLen || !items)
		usage();

	type_t t;
	if (!strcmp(type, "sequence1"))
		t = SEQUENCE1;
	else if (!strcmp(type, "sequence2"))
		t = SEQUENCE2;
	else if (!strcmp(type, "random"))
		t = RANDOM;
	else
		usage();

	void *algo = algo_new(keyLen, items);
	int i;
	unsigned char s[keyLen+1];
	memset(s, 0, keyLen+1);
	struct timeval start_insert;
	gettimeofday(&start_insert, NULL);
	for (i = 0; i < items; i++) {
		switch (t) {
		case SEQUENCE1:
			sequence_key1(s, keyLen, i);
			break;
		case SEQUENCE2:
			sequence_key2(s, keyLen, i);
			break;
		case RANDOM:
			random_key(s, keyLen);
			break;
		}
		algo_insert(algo, keyLen, s, i);
	}
	struct timeval finish_insert;
	gettimeofday(&finish_insert, NULL);

	struct timeval start_lookup;
	gettimeofday(&start_lookup, NULL);
	for (i = 0; i < items; i++) {
		switch (t) {
		case SEQUENCE1:
			sequence_key1(s, keyLen, i);
			break;
		case SEQUENCE2:
			sequence_key2(s, keyLen, i);
			break;
		case RANDOM:
			random_key(s, keyLen);
			break;
		}
		algo_lookup(algo, keyLen, s);
	}
	struct timeval finish_lookup;
	gettimeofday(&finish_lookup, NULL);

	struct proc_t usage;
	look_up_our_self(&usage);

	int64_t insert_time = (int64_t) (finish_insert.tv_sec - start_insert.tv_sec) * 1000000 + (finish_insert.tv_usec - start_insert.tv_usec);
	int64_t lookup_time = (int64_t) (finish_lookup.tv_sec - start_lookup.tv_sec) * 1000000 + (finish_lookup.tv_usec - start_lookup.tv_usec);
	printf("%lf\t%lf\t%ld\n", (double)insert_time/items, (double)lookup_time/items, usage.vsize / (1024*1024));

	//getc(stdin);

	algo_free(algo, keyLen);


	return 0;
}
