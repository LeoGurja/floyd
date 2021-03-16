#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define INF 99999

int *init_matrix(int m, int n)
{
  int *matrix = (int *)malloc(m * n * sizeof(int));

  return matrix;
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

int main(int argc, char **argv)
{
  clock_t start, end;
  int n = 400;

  start = clock();
  int *matrix = rand_matrix(n);
  floydWarshall(matrix, n);
  end = clock();
  printf("\nTempo de execução - %.2f ms", ((double)end - start) * 1000 / CLOCKS_PER_SEC);
  return 0;
}