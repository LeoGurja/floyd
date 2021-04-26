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
  int n = atoi(argv[1]);

  int **matrix = init_matrix(n, n);

  read_matrix_from_file(n, matrix, argv[2]);
  floydWarshall(matrix, n);
  return 0;
}