all: hash trie trie_judy_hs trie_judy_sl

hash: hash.cc
	g++ -Wall -O3 $< -o $@ -lproc

trie: trie.c trie.h
	gcc -Wall -O3 $< -o $@ -lproc

trie_judy_hs: trie_judy_hs.c
	gcc -Wall -O3 $< -o $@ -lproc -lJudy

trie_judy_sl: trie_judy_sl.c
	gcc -Wall -O3 $< -o $@ -lproc -lJudy
