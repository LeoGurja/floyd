#include "include/utils.h"

int read_matrix_from_file(size_t rows, size_t cols, int (*a)[cols], const char *filename)
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