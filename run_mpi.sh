#!/bin/zsh

for n in 4 10 20 50 100 200 500 1000 2000; do
  echo "--- n = $n ---\nsingle\n" >> test.log
  for i in 1 2 3; do
    ./single.out $n "benchmark/matrix_$n" >> test.log
  done
  for p in 2 4 6 8; do
    echo "\np=$p" >> test.log
    for i in 1 2 3; do
      mpirun --oversubscribe -n $p ./main.out $n "benchmark/matrix_$n" >> test.log
    done
  done
done