#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/matrix.h"
#include "include/utils.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))


void floydWarshall(int **matrix, int matrix_size) {
    for (int k = 0; k < matrix_size; k++) {
		#pragma omp parallel for schedule(dynamic)
 		for (int i = 0; i < matrix_size; i++) {
    		for (int j = 0; j < matrix_size; j++) {
      			matrix[i][j] = MIN(matrix[i][j], matrix[i][k] + matrix[k][j]);
    		}
  		}
	}   
}

void main(int argc, char **argv)
{
  int n = atoi(argv[1]);
  int **matrix;

  matrix = init_matrix(n, n);
  read_matrix_from_file(n, matrix, argv[2]);
  //fprint_matrix("input.log", matrix, n);

  floydWarshall(matrix, n);

  //fprint_matrix("output.log", matrix, n);
}