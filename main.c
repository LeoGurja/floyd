#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define INF 99999
#define TRUE 1
#define FALSE 0
#define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
#define BLOCK_SIZE(id, p, n) ((n) / (p))

void floydWarshall(int *matrix, int n)
{
  for (int k = 0; k < n; k++)
  {
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        matrix[i * n + j] = MIN(matrix[i * n + j], matrix[i * n + k] + matrix[k * n + j]);
      }
    }
  }
}

int *init_matrix(int m, int n)
{
  int *matrix = (int *)malloc(m * n * sizeof(int));

  return matrix;
}

int *copy_matrix(int *matrix, int m, int n)
{
  int *copy = init_matrix(m, n);

  for (int i = 0; i < m * n; i++)
    copy[i] = matrix[i];

  return copy;
}

void print_matrix(int *matrix, int m, int n)
{
  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < n; j++)
    {
      printf("%d\t", matrix[i * n + j]);
    }
    printf("\n");
  }
  printf("\n");
}

int *rand_matrix(int n)
{
  int *matrix = init_matrix(n, n);
  int random;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (i == j)
      {
        matrix[i * n + j] = 0;
      }
      random = (rand() % 11) + 1;
      matrix[i * n + j] = random == 11 ? INF : random;
    }
  }
  return matrix;
}

int compare_matrix(int *a, int *b, int n)
{
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (a[i * n + j] != b[i * n + j])
        return FALSE;
  return TRUE;
}

void apply_to_matrix(int *matrix, int *block, int id, int size, int n)
{
  int block_size = BLOCK_SIZE(id, size, n);
  int offset = BLOCK_LOW(id, size, n);
  for (int i = 0; i < block_size; i++)
  {
    for (int j = 0; j < n; j++)
    {
      matrix[(i + offset) * n + j] = block[i * n + j];
    }
  }
}

void master(int size, int n)
{
  int *matrix = rand_matrix(n);
  // int *matrix2 = copy_matrix(matrix, n, n);
  int block_size = BLOCK_SIZE(0, size, n);
  int *block = init_matrix(block_size, n);
  double start_time = MPI_Wtime();
  for (int k = 0; k < n; k++)
  {
    MPI_Bcast(matrix, n * n, MPI_INT, size, MPI_COMM_WORLD);
    for (int i = 0; i < size; i++)
    {
      MPI_Recv(block, block_size * n, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      apply_to_matrix(matrix, block, i, size, n);
    }
  }
  double final_time = (MPI_Wtime() - start_time) * 1000;
  printf("\nTempo de execução - %f ms", final_time);
  printf("\nSpeedup (1) - %f", (885) / final_time);
  printf("\nSpeedup (2) - %f", (700) / final_time);
  printf("\nSpeedup (4) - %f", (763) / final_time);
  printf("\nSpeedup (8) - %f", (1227) / final_time);
  printf("\nEfficiency - %f", final_time / ((size + 1) * 1000)); //https://docs.google.com/document/d/1l1YRBnlMw-8qpL9hVnmP4raD6ntHygoT7d_aAE4nFlc/edit#
  // double start_time_2 = MPI_Wtime();
  // floydWarshall(matrix2, n);
  // printf("\nTempo serial - %f segundos", MPI_Wtime() - start_time_2);
  // printf("\nResultados Obtidos\n\n");

  // printf("\n%s\n", compare_matrix(matrix, matrix2, n) ? "SUCESSO" : "ERRO");
}

void worker(int rank, int size, int n)
{
  int *matrix = init_matrix(n, n);
  int offset = BLOCK_LOW(rank, size, n);
  int block_size = BLOCK_SIZE(rank, size, n);
  int *block = init_matrix(block_size, n);
  for (int k = 0; k < n; k++)
  {
    MPI_Bcast(matrix, n * n, MPI_INT, size, MPI_COMM_WORLD);
    for (int i = 0; i < block_size; i++)
    {
      for (int j = 0; j < n; j++)
      {
        block[i * n + j] = MIN(matrix[(i + offset) * n + j], matrix[(i + offset) * n + k] + matrix[k * n + j]);
      }
    }

    MPI_Send(block, block_size * n, MPI_INT, size, 0, MPI_COMM_WORLD);
  }
}

int main(int argc, char **argv)
{
  int size;
  int rank;
  int n = 512;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == size - 1)
  {
    master(size - 1, n);
  }
  else
  {
    worker(rank, size - 1, n);
  }

  MPI_Finalize();
  return 0;
}