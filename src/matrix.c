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

int **copy_matrix(int **matrix, int n)
{
  int **copy = init_matrix(n, n);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      copy[i][j] = matrix[i][j];

  return copy;
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

void print_matrix(int **matrix, int n)
{
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (matrix[i][j] == INF)
      {
        printf("INF\t");
      }
      else
      {
        printf("%d\t", matrix[i][j]);
      }
    }
    printf("\n");
  }
  printf("\n");
}

void println(int *line, int n)
{
  for (int i = 0; i < n; i++)
  {
    if (line[i] == INF)
    {
      printf("INF\t");
    }
    else
    {
      printf("%d\t", line[i]);
    }
  }
  printf("\n");
}

void fprintln(int *line, int n, FILE *output)
{
  for (int i = 0; i < n; i++)
  {
    if (line[i] == INF)
    {
      fprintf(output, "INF\t");
    }
    else
    {
      fprintf(output, "%d\t", line[i]);
    }
  }
  fprintf(output, "\n");
}

void fprint_matrix(int **matrix, int m, int n, FILE *output)
{
  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if (matrix[i][j] == INF)
      {
        fprintf(output, "INF\t");
      }
      else
      {
        fprintf(output, "%d\t", matrix[i][j]);
      }
    }
    fprintf(output, "\n");
  }
  fprintf(output, "\n");
}