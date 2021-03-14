#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/matrix.h"

#define BLOCK_SIZE(matrix_size, number_of_processes) ((matrix_size) / (number_of_processes))
#define BLOCK_START(rank, number_of_processes, matrix_size) ((rank)*BLOCK_SIZE(matrix_size, number_of_processes))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define SEP "===================================================="

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

// [[1,2], [2,4]]  = [1,2,2,4]
int *serialize_block(int **matrix, int *lines, int matrix_size, FILE *output)
{
  int *block = (int *)malloc(sizeof(int) * (lines[1] - lines[0]) * matrix_size);
  fprintf(output, "SERIALIZANDO - iniciou bloco de tamanho %d\n", (lines[1] - lines[0]) * matrix_size);
  int counter = 0;
  for (int i = lines[0]; i < lines[1]; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      block[counter] = matrix[i][j];
      counter++;
    }
  }
  fprintf(output, "SERIALIZANDO - Printando o bloco serializado\n");
  fprintln(block, (lines[1] - lines[0]) * matrix_size, output);
  return block;
}

int **deserialize_block(int *block, int *lines, int matrix_size, FILE *output)
{
  int block_size = lines[1] - lines[0];
  fprintf(output, "DESSERIALIZANDO - deserializando bloco de %d linhas e %d colunas\n", block_size, matrix_size);
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
  fprintf(output, "DESSERIALIZANDO - Printando o bloco desserializado\n");
  fprint_matrix(deserialized, (lines[1] - lines[0]), matrix_size, output);
  return deserialized;
}

// Etapa de cálculo de sua sub-matriz (bloco)
void calculate(int **matrix, int *lines, int k, int matrix_size, FILE *output)
{
  for (int i = lines[0]; i < lines[1]; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      fprintf(output, "matrix[%d][%d] = %d\n", i, j, MIN(matrix[i][j], matrix[i][k] + matrix[k][j]));
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

  fprintf(output, "INSTANTE 0\n");
  for (int i = 0; i < number_of_processes; i++)
  {
    lines = block_line(i, number_of_processes, matrix_size);
    fprintf(output, "INSTANTE 1 i(%d) \n", i);
    if (i == rank)
    {
      block = serialize_block(matrix, lines, matrix_size, output);
    }
    else
    {
      block = (int *)malloc(sizeof(int) * matrix_size * (lines[1] - lines[0]));
    }
    fprintf(output, "INSTANTE 2 i(%d)\n", i);
    //print_matrix(matrix, matrix_size);
    //fflush(stdout);
    //Envia as alterações locais e recebe as globais
    fprintf(output, "antes do bcast matrix[0][0] = %d / matrix_size * (lines[1] - lines[0]) = %d\n", matrix[0][0], matrix_size * (lines[1] - lines[0]));
    MPI_Bcast(block, matrix_size * (lines[1] - lines[0]), MPI_INT, i, MPI_COMM_WORLD);
    fprintf(output, "depois do bcast matrix[0][0] = %d\n", matrix[0][0]);

    fprintf(output, "INSTANTE 3 i(%d)\n", i);
    /*
    * 0 5 7 2
    * 4 0 3 1
    * 3 1 0 1
    * 1 5 9 0
    */

    // começo do problema
    if (i != rank)
    {
      fprintf(output, "dentro do não dono do bloco matrix[0][0] = %d\n", matrix[0][0]);
      fprintf(output, "INSTANTE 4 i(%d)\n", i);
      tmp = deserialize_block(block, lines, matrix_size, output);
      fprintf(output, "INSTANTE 5 i(%d)\n", i);
      fprintf(output, "após deserialização matrix[0][0] = %d\n", matrix[0][0]);
      fflush(output);
      for (int line = 0; line < lines[1] - lines[0]; line++)
      {
        for (int j = 0; j < matrix_size; j++)
        {
          fprintf(output, "INSTANTE 6 i(%d) - lines[0] = %d / line = %d / j = %d\n", i, lines[0], line, j);
          matrix[(lines[0] + line)][j] = tmp[line][j];
        }
      }
    }
    // fim do problema
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
    fprintf(output, "começando sync de k=%d\n", k);
    sync(matrix, matrix_size, rank, number_of_processes, output);
    fprintf(output, "terminado sync de k=%d\n", k);
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

int main(int argc, char **argv)
{
  double time, max_time;
  int number_of_processes;
  int rank;
  int n = 4;

  int **matrix = rand_matrix(n);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /*int *lines = block_line(rank, number_of_processes, n);
  printf(
      "sou o p%d -> %d até %d\n",
      rank, lines[0], lines[1]);
  */

  MPI_Barrier(MPI_COMM_WORLD);
  time = -MPI_Wtime();
  floydWarshall(matrix, number_of_processes, rank, n);
  time += MPI_Wtime();
  MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  MPI_Finalize();
  return 0;
}