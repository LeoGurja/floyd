#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/include/matrix.h"
#include "src/include/utils.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define INF 99999

void floydWarshall(int **matrix, int n)
{
  for (int k = 0; k < n; k++)
  {
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {
        matrix[i][j] = MIN(matrix[i][j], matrix[i][k] + matrix[k][j]);
      }
    }
  }
}

int main(int argc, char **argv)
{
  clock_t start, end;
  int n = atoi(argv[1]);

  start = clock();
  int **matrix = init_matrix(n, n);

  read_matrix_from_file(n, matrix, argv[2]);
  floydWarshall(matrix, n);
  end = clock();
  printf("\nTempo de execução - %.2f ms", ((double)end - start) * 1000 / CLOCKS_PER_SEC);
  fflush(stdout);
  return 0;
}