# Floyd

O código do MPI se encontra na branch master
O código do OpenMP está na branch omp

# Execução
## Single Thread
Para executar o single_thread
substitua n pelo tamanho de matriz desejado
```bash
  make single
  n=4
  ./single.out $n "benchmark/matrix_$n"
```

## MPI
substitua n pelo tamanho de matriz desejado
substitua p pelo número de processos desejado
```bash
  make
  n=4
  p=2
  mpirun --oversubscribe -n $p main.out $n "benchmark/matrix_$n"
```

## OpenMP
substitua n pelo tamanho de matriz desejado
substitua p pelo número de threads desejado
```bash
  make single
  n=4
  p=2
  OMP_NUM_THREADS=$p ./main.out $n benchmark/matrix_$n
```

