#include "include/utils.h"

void read_matrix_from_file(int matrix_size, int **a, const char *filename)
{
  int tmp;
  FILE *pf;
  pf = fopen(filename, "r");
  if (pf == NULL)
    exit(1);

  for (int i = 0; i < matrix_size; ++i)
  {
    for (int j = 0; j < matrix_size; ++j)
    {
      fscanf(pf, "%d ", &tmp);
      a[i][j] = tmp;
    }
  }

  fclose(pf);
}