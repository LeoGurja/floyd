#include "include/matrix.h"

// inicia uma matriz vazia
int **init_matrix(int m, int n)
{
  int **matrix = (int **)malloc(sizeof(int *) * m);
  for (int i = 0; i < m; i++)
  {
    matrix[i] = (int *)malloc(sizeof(int) * n);
  }

  return matrix;
}

int **rand_matrix(int n)
{
  int **matrix = init_matrix(n, n);
  int random;

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (i == j)
      {
        matrix[i][j] = 0;
      }
      else
      {
        random = (rand() % 11) + 1;
        matrix[i][j] = random == 11 ? INF : random;
      }
    }
  }
  return matrix;
}

void fprint_matrix(char *filename, int **matrix, int matrix_size)
{
  FILE *fptr = fopen(filename, "w");
  for (int i = 0; i < matrix_size; i++)
  {
    for (int j = 0; j < matrix_size; j++)
    {
      if (matrix[i][j] == INF)
      {
        fprintf(fptr, "INF\t");
      }
      else
      {
        fprintf(fptr, "%d\t", matrix[i][j]);
      }
    }
    fprintf(fptr, "\n");
  }
  fclose(fptr);
}