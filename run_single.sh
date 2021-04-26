#!/bin/bash
rm single.log

for n in 4 10 20 50 100 200 500 1000 2000; do
  echo "--- n = $n ---\nsingle\n" >> single.log
  for i in 1 2 3; do
    { time ./single.out $n "benchmark/matrix_$n"; } &>> single.log
  done
  echo "" >> single.log
done