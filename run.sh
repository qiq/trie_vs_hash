#!/usr/bin/env bash

echo -e "random\torder\tlen\titems\tt_insert\tt_lookup\tt_memory\th_insert\th_lookup\th_memory" >hash_vs_trie.csv
for random in 0 1; do
	for order in 0 1; do
		for len in 4 8 16; do
			for items in 10000 50000 100000 500000 1000000 5000000 10000000; do
				echo -ne "$random\t$order\t$len\t$items" >>hash_vs_trie.csv
				for type in trie hash; do 
					a=0; b=0; c=0;
					for i in 1 2 3 4 5 6 7 8 9 10; do
						set -- `./$type $len $items $order $random || echo -e "0\t0\t0"`
						a=$(($a+$1))
						b=$(($b+$2))
						c=$(($c+$3))
					done
					a=$(($a/10))
					b=$(($b/10))
					c=$(($c/10))
					echo -e "\t$a\t$b\t$c" >>hash_vs_trie.csv
				done
				echo >>result_$type.csv
			done
		done
	done
done
