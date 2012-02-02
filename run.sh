#!/usr/bin/env bash

(
	echo -ne "type\tkey_len\titems"
	echo -ne "\tt_insert\tt_lookup\tt_memory"
	echo -ne "\th_insert\th_lookup\th_memory"
	echo -ne "\ttsl_insert\ttsl_lookup\ttsl_memory"
	echo -ne "\tths_insert\tths_lookup\tths_memory"
	echo
) >trie_vs_hash.csv
for sequence in sequence1 sequence2 random; do
	#for len in 4 8 16; do
	for len in 36; do
		for items in 10000 50000 100000 500000 1000000 5000000 10000000; do
			echo -ne "$sequence\t$len\t$items" >>trie_vs_hash.csv
			for type in trie_fixed hash_stl trie_judy_sl trie_judy_hs; do 
				a=0; b=0; c=0;
				for i in 1 2 3 4 5; do
					set -- `./$type $len $items "$sequence" || echo -e "0\t0\t0"`
					a=$(echo "$a+$1"|bc -l)
					b=$(echo "$b+$2"|bc -l)
					c=$(echo "$c+$3"|bc -l)
				done
				a=$(echo "$a/5"|bc -l)
				b=$(echo "$b/5"|bc -l)
				c=$(echo "$c/5"|bc -l)
				if [ $a = 0 -a $b = 0 -a $c = 0 ]; then
					echo -ne "\tNA\tNA\tNA" >>trie_vs_hash.csv
				else
					printf "\t%.3f\t%.3f\t%.1f" $a $b $c >>trie_vs_hash.csv
				fi
			done
			echo >>trie_vs_hash.csv
		done
	done
done
