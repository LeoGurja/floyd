#!/bin/bash
rm mpi.log

for n in 4 10 20 50 100 200 500 1000 2000; do
  echo "--- n = $n ---" >> mpi.log
  for p in 2 4 6 8; do
    echo "p=$p" >> mpi.log
    for i in 1 2 3; do
      { time mpirun --oversubscribe -n $p main.out $n "benchmark/matrix_$n"; } &>> mpi.log
    done
	echo "" >> mpi.log
  done
done