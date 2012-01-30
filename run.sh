#!/usr/bin/env bash

(
	echo -ne "random\torder\tlen\titems"
	echo -ne "\tt_insert\tt_lookup\tt_memory"
	echo -ne "\th_insert\th_lookup\th_memory"
	echo -ne "\ttsl_insert\ttsl_lookup\ttsl_memory"
	echo -ne "\tths_insert\tths_lookup\tths_memory"
	echo
) >trie_vs_hash.csv
for random in 0 1; do
	for order in 0 1; do
		#for len in 4 8 16; do
		for len in 8; do
			for items in 10000 50000 100000 500000 1000000 5000000 10000000; do
				echo -ne "$random\t$order\t$len\t$items" >>trie_vs_hash.csv
				for type in trie hash trie_judy_sl trie_judy_hs; do 
					a=0; b=0; c=0;
					for i in 1 2 3 4 5; do
						set -- `./$type $len $items $order $random || echo -e "0\t0\t0"`
						a=$(($a+$1))
						b=$(($b+$2))
						c=$(($c+$3))
					done
					a=$(($a/10))
					b=$(($b/10))
					c=$(($c/10))
					echo -ne "\t$a\t$b\t$c" >>trie_vs_hash.csv
				done
				echo >>trie_vs_hash.csv
			done
		done
	done
done
