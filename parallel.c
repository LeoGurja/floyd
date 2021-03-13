#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PROCESS_I(rank, number_of_processes) ((rank) / (int)sqrt((double)number_of_processes))
#define PROCESS_J(rank, number_of_processes) ((rank) % (int)sqrt((double)number_of_processes))
#define PROCESS_LINES(number_of_processes) ((number_of_processes - 1) / (int)sqrt((double)number_of_processes) + 1)
#define PROCESS_COLUMNS(number_of_processes) ((number_of_processes - 1) % (int)sqrt((double)number_of_processes) + 1)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/*
p0(0,0) p1(0,1) 
p2(1,0) p3(1,1) 
p4(2,0) p5(2,1) 

3 comm bcast linha
2 comm bcast coluna

0 6 3 | 7 2
2 0 7 | 8 4
------------- 
2 7 0 | 8 7 
2 5 8 | 0 1
------------- 
2 5 2 | 2 0
*/

// Para o método de divisão de blocos, nos baseamos na referência abaixo
// https://en.wikipedia.org/wiki/Parallel_all-pairs_shortest_path_algorithm#Parallelization

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

void floydWarshall(int *matrix, int number_of_processes, int rank, int matrix_size)
{
  /*
   * Cada processo p(i,j) envia seu bloco para os processos de mesmo i OU mesmo j
   * cada processo espera para receber os segmentos necessários
   * cada processo computa sua parte da matriz(k)
   * 
   * |    |######|     |
   * |    |######|     |
   * -----+------+------
   * |####|p(i,j)|#####|
   * -----+------+------
   * |    |######|     |
   * |    |######|     |
   */
  int *lines = block_line(rank, number_of_processes, matrix_size);
  int *columns = block_line(rank, number_of_processes, matrix_size);
  for (int k = 0; k < matrix_size; k++)
  {
    for (int i = 0; i < matrix_size; i++)
    {
      for (int j = 0; j < matrix_size; j++)
      {
        matrix[i * n + j] = MIN(matrix[i * n + j], matrix[i * n + k] + matrix[k * n + j]);
      }
    }
  }
}

void free_communicators(MPI_Comm *communicators_list, int communicators_list_size)
{
  for (int index = 0; index < communicators_list; index++)
  {
    MPI_Comm_free(&communicators_list[index]);
  }
}

int main(int argc, char **argv)
{
  int number_of_processes;
  int rank;
  int n = 128;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Comm *line_communicators = (MPI_Comm *)malloc(sizeof(MPI_Comm) * PROCESS_LINES(number_of_processes));
  MPI_Comm *column_communicators = (MPI_Comm *)malloc(sizeof(MPI_Comm) * PROCESS_COLUMNS(number_of_processes));

  int *lines = block_line(rank, number_of_processes, n);
  int *columns = block_column(rank, number_of_processes, n);

  // printf(
  //  "sou o p%d(%d, %d) -> (%d, %d) até (%d, %d)\n",
  //  rank, PROCESS_I(rank, number_of_processes), PROCESS_J(rank, number_of_processes), lines[0], columns[0], lines[1], columns[1]
  // );
  // floydWarshall(matrix, number_of_processes, rank, n);

  free_communicators(line_communicators, PROCESS_LINES(number_of_processes));
  free_communicators(column_communicators, PROCESS_COLUMNS(number_of_processes));

  MPI_Finalize();
  return 0;
}
