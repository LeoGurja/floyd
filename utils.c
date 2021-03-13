#include <stdio.h>

int readmatrix(size_t rows, size_t cols, int (*a)[cols], const char *filename)
{

  FILE *pf;
  pf = fopen(filename, "r");
  if (pf == NULL)
    return 0;

  for (size_t i = 0; i < rows; ++i)
  {
    for (size_t j = 0; j < cols; ++j)
      fscanf(pf, "%d", a[i] + j);
  }

  fclose(pf);
  return 1;
}

int main_example(void)
{
  int matrix[2][3];

  readmatrix(2, 3, matrix, "m1.matrix");

  for (size_t i = 0; i < 2; ++i)
  {
    for (size_t j = 0; j < 3; ++j)
      printf("%-3d ", matrix[i][j]);
    puts("");
  }

  return 0;
}