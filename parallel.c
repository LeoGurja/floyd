#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PROCESS_I(rank, number_of_processes) ((rank) / (int)sqrt((double)number_of_processes))
#define PROCESS_J(rank, number_of_processes) ((rank) % (int)sqrt((double)number_of_processes))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/*
i = rank / (int)sqrt(number_of_processes)
j = (rank) % (int)sqrt(number_of_processes)

p0(0,0) p1(0,1) 
p2(1,0) p3(1,1) 
p4(2,0) p5(2,1)

0 6 3 | 7 2
2 0 7 | 8 4
------------- 
2 7 0 | 8 7 
2 5 8 | 0 1
------------- 
2 5 2 | 2 0
*/

//https://en.wikipedia.org/wiki/Parallel_all-pairs_shortest_path_algorithm#Parallelization

// Retorna o índice de início e final da linha do bloco
int *block_line(int rank, int number_of_processes, int matrix_size)
{
  int *list = (int *)malloc(sizeof(int) * 2);
  int pi = PROCESS_I(rank, number_of_processes);
  int max_size = matrix_size / ((int)sqrt((double)number_of_processes));
  list[0] = (pi * max_size);
  list[1] = MIN(((pi + 1) * max_size), matrix_size);
  return list;
}

// Retorna o índice de início e final da coluna do bloco
int *block_column(int rank, int number_of_processes, int matrix_size)
{
  int *list = (int *)malloc(sizeof(int) * 2);
  int pj = PROCESS_J(rank, number_of_processes);
  int max_size = ceil((double)(matrix_size) / sqrt((double)number_of_processes));
  list[0] = (pj * max_size);
  list[1] = MIN(((pj + 1) * max_size), matrix_size);
  return list;
}

int main(int argc, char **argv)
{
  int size;
  int rank;
  int n = 5;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int *lines = block_line(rank, size, n);
  int *columns = block_column(rank, size, n);

  printf("sou o p%d(%d, %d) -> (%d, %d) até (%d, %d)\n", rank, PROCESS_I(rank, size), PROCESS_J(rank, size), lines[0], columns[0], lines[1], columns[1]);

  //parallel(size, rank, n);

  MPI_Finalize();
  return 0;
}
