all: trie_fixed hash_stl trie_judy_hs trie_judy_sl

test.o: test.cc algo.h
	g++ -c -Wall -O0 -ggdb $(CFLAGS) $< -o $@

trie_fixed.o: trie_fixed.c algo.h
	gcc -c -Wall -O0 -ggdb $(CFLAGS) $< -o $@

trie_fixed: test.o trie_fixed.o
	g++ $^ -o $@ $(CFLAGS) -lproc -ggdb

hash_stl.o: hash_stl.cc
	g++ -c -Wall -O0 -ggdb $(CFLAGS) $< -o $@

hash_stl: test.o hash_stl.o
	g++ $^ -o $@ $(CFLAGS) -lproc -ggdb

trie_judy_hs.o: trie_judy_hs.c algo.h
	gcc -c -Wall -O0 -ggdb $(CFLAGS) $< -o $@

trie_judy_hs: test.o trie_judy_hs.o
	g++ $^ -o $@ -ggdb $(CFLAGS) -lproc -lJudy

trie_judy_sl.o: trie_judy_sl.c algo.h
	gcc -c -Wall -O0 -ggdb $(CFLAGS) $< -o $@

trie_judy_sl: test.o trie_judy_sl.o
	g++ $^ -o $@ -ggdb $(CFLAGS) -lproc -lJudy

clean:
	rm -f test.o \
	trie_fixed trie_fixed.o \
	hash_stl hash_stl.o \
	trie_judy_hs trie_judy_hs.o \
	trie_judy_sl trie_judy_sl.o \
