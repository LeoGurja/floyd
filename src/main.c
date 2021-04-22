#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/matrix.h"
#include "include/utils.h"

#define BLOCK_SIZE(matrix_size, number_of_processes) ((matrix_size) / (number_of_processes))
#define BLOCK_START(rank, number_of_processes, matrix_size) ((rank)*BLOCK_SIZE(matrix_size, number_of_processes))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/*
p0(0 até 3)
p1(3 até 5)
p2(5 até 6)

0 6 3 7 2
---------
2 0 7 8 4
---------
2 7 0 8 7 
---------
2 5 8 0 1
2 5 2 2 0
*/

// Retorna o índice de início e final do bloco
int *block_line(int rank, int number_of_processes, int matrix_size)
{
  int *list = (int *)malloc(sizeof(int) * 2);
  int block_start = BLOCK_START(rank, number_of_processes, matrix_size);
  int max_size = BLOCK_SIZE(matrix_size, number_of_processes);
  list[0] = block_start;
  list[1] = rank == number_of_processes - 1 ? (matrix_size) : (block_start + max_size);
  return list;
}

// Etapa de cálculo de sua sub-matriz (bloco)
void calculate(int **matrix, int k, int matrix_size)
{
  int id = omp_get_thread_num();
  int num_threads = omp_get_num_threads();
  int *lines = block_line(id, num_threads, matrix_size);
  for (int i = lines[0]; i < lines[1]; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      matrix[i][j] = MIN(matrix[i][j], matrix[i][k] + matrix[k][j]);
    }
  }
}

void floydWarshall(int **matrix, int matrix_size)
{
  // Percorre todas as k possibilidades de caminho intermediário
  for (int k = 0; k < matrix_size; k++)
  {
#pragma omp parallel shared(matrix, k, matrix_size)
    {
      calculate(matrix, k, matrix_size);
    }
  }
}
/*

P1 ++ -- ++ --
P2 ++ -- ++ --
P3 ++ -- ++ --
P4 ++ -- ++ --

P1 ++ -- ++ --
P2 -- ++ -- ++ --
P3 -- -- ++ -- ++ --
P4 -- -- -- ++ -- ++ --

*/

void main(int argc, char **argv)
{
  double time, max_time;
  int n = atoi(argv[1]);
  int **matrix;
  int num_threads = omp_get_num_threads();

  matrix = init_matrix(n, n);
  read_matrix_from_file(n, matrix, argv[2]);
  fprint_matrix("input.log", matrix, n);

  time = -omp_get_wtime();

  floydWarshall(matrix, n);
  time += omp_get_wtime();

  fprint_matrix("output.log", matrix, n);
  printf("\nTempo de execução - %lfms\n", max_time * 1000);
}