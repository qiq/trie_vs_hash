all: hash trie

hash: hash.cc
	g++ -Wall -O3 $< -o $@ -lproc

trie: trie.c
	gcc -Wall -O3 $< -o $@ -lproc
