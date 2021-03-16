#include <mpi.h>
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

// Transforma um bloco bi-dimensional em um array uni-dimensional
// [[1,2], [2,4]]  => [1,2,2,4]
int *serialize_block(int **matrix, int *lines, int matrix_size, FILE *output)
{
  int *block = (int *)malloc(sizeof(int) * (lines[1] - lines[0]) * matrix_size);
  int counter = 0;
  for (int i = lines[0]; i < lines[1]; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      block[counter] = matrix[i][j];
      counter++;
    }
  }
  return block;
}

// Transforma um array uni-dimensional em um bloco uni-dimensional
int **deserialize_block(int *block, int *lines, int matrix_size, FILE *output)
{
  int block_size = lines[1] - lines[0];
  int **deserialized = init_matrix(block_size, matrix_size);
  int counter = 0;
  for (int i = 0; i < block_size; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      deserialized[i][j] = block[counter];
      counter++;
    }
  }
  return deserialized;
}

// Etapa de cálculo de sua sub-matriz (bloco)
void calculate(int **matrix, int *lines, int k, int matrix_size, FILE *output)
{
  for (int i = lines[0]; i < lines[1]; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      matrix[i][j] = MIN(matrix[i][j], matrix[i][k] + matrix[k][j]);
    }
  }
}

// Etapa de sincronização das linhas
void sync(int **matrix, int matrix_size, int rank, int number_of_processes, FILE *output)
{
  int *block;
  int **tmp;
  int *lines;

  for (int i = 0; i < number_of_processes; i++)
  {
    lines = block_line(i, number_of_processes, matrix_size);

    // Caso seja o dono do bloco
    if (i == rank)
    {
      // Serializa o bloco para enviar aos outros processos
      block = serialize_block(matrix, lines, matrix_size, output);
    }
    else
    {
      // Aloca memória pra receber um bloco de outro processo
      block = (int *)malloc(sizeof(int) * matrix_size * (lines[1] - lines[0]));
    }
    //Envia as alterações locais e recebe as globais
    MPI_Bcast(block, matrix_size * (lines[1] - lines[0]), MPI_INT, i, MPI_COMM_WORLD);

    /*
    * 0 5 7 2
    * 4 0 3 1
    * 3 1 0 1
    * 1 5 9 0
    */

    // Caso não seja o dono do bloco
    if (i != rank)
    {
      // Desserializa e insere os valores novos na matriz
      tmp = deserialize_block(block, lines, matrix_size, output);
      for (int line = 0; line < lines[1] - lines[0]; line++)
      {
        for (int j = 0; j < matrix_size; j++)
        {
          matrix[(lines[0] + line)][j] = tmp[line][j];
        }
      }
    }
  }
}

void floydWarshall(int **matrix, int number_of_processes, int rank, int matrix_size)
{
  // Calcula o início e fim do seu bloco
  int *lines = block_line(rank, number_of_processes, matrix_size);
  FILE *output = rank == 0 ? stdout : stderr;

  // Percorre todas as k possibilidades de caminho intermediário
  for (int k = 0; k < matrix_size; k++)
  {
    calculate(matrix, lines, k, matrix_size, output);
    sync(matrix, matrix_size, rank, number_of_processes, output);
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
  int number_of_processes;
  int rank;
  int n = 500;
  int **matrix;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  matrix = init_matrix(n, n);
  read_matrix_from_file(n, matrix, "benchmark/matrix06_500");
  if (rank == 0)
    fprint_matrix("input.log", matrix, n);

  MPI_Barrier(MPI_COMM_WORLD);
  time = -MPI_Wtime();
  floydWarshall(matrix, number_of_processes, rank, n);
  time += MPI_Wtime();
  MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    fprint_matrix("output.log", matrix, n);
    printf("\nTempo de execução - %lfms\n", max_time * 1000);
  }

  MPI_Finalize();
}